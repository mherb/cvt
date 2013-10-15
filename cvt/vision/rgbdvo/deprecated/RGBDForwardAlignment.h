/*
   The MIT License (MIT)

   Copyright (c) 2011 - 2013, Philipp Heise and Sebastian Klose

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/


#ifndef RGBDFORWARDALIGNMENT_H
#define RGBDFORWARDALIGNMENT_H

#include <vector>
#include <cvt/vision/rgbdvo/RGBDTemplate.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/gfx/IKernel.h>
#include <cvt/math/SE3.h>

namespace cvt
{
    template <typename T>
    class RGBDForwardAlignment {
        public:
            typedef typename RGBDTemplate<T>::Params TemplateParams;

            struct Params {
                Params() :
                    octaves( 4 ),
                    octaveScale( 0.5 ),
                    minParamUpdate( 1e-7 ),
                    maxIterationsPerScale( 40 )
                {
                }

                size_t                  octaves;
                T                       octaveScale;
                T                       minParamUpdate;
                size_t                  maxIterationsPerScale;
                TemplateParams          templateParams;
            };

            RGBDForwardAlignment( const Matrix3<T>& intrinsics );
            ~RGBDForwardAlignment();

            /**
             *  \brief align the given images with the internally stored template data
             *  \param pose the initial/predicted Transformation from template TO the current
             *  \param gray the current grayscale image (float)
             *  \param depth the depth image (float)
             *  \return in pose, the estimated pose from the template space to the current image
             */
            void alignFrames( Matrix4<T>& pose, const Image& gray, const Image& depth );

        private:
            Params                           _params;
            std::vector<Matrix3<T> >         _calibForScale;
            std::vector<RGBDTemplate<T> >    _templateForScale;

            IKernel                          _kx;
            IKernel                          _ky;
            IKernel                          _gaussX;
            IKernel                          _gaussY;

            ImagePyramid                     _currentImagePyramid;
            bool                             _initial;

            void computeImageGradients( Image& gx, Image& gy, const Image& gray ) const
            {
                gx.reallocate( gray.width(), gray.height(), IFormat::GRAY_FLOAT );
                gy.reallocate( gray.width(), gray.height(), IFormat::GRAY_FLOAT );

                // sobel
                gray.convolve( gx, _kx, _gaussY );
                gray.convolve( gy, _gaussX, _ky );

                // normal
                //gray.convolve( gx, kx );
                //gray.convolve( gy, ky );
            }

            void updateTemplateData( const Image& depth, const Matrix4<T>& pose );

            void transformPoints( Vector3<T>* warped3d,
                                  Vector2<T>* warped2d,
                                  const Vector3<T>* pts3d,
                                  const Matrix4<T> & mat,
                                  const Matrix3<T> K,
                                  size_t n );

            void computeMixingCoeffs( Vector2f& mix, const Vector2<T>& pos ) const
            {
                int lx = ( int )pos.x;
                int ly = ( int )pos.y;
                mix.x = ( float )pos.x - lx;
                mix.y = ( float )pos.y - ly;
            }

            float interpolatePixelValue( const float* p0, const float* p1, const Vector2f& mix ) const
            {
                float v0 = Math::mix( p0[ 0 ], p0[ 1 ], mix.x );
                float v1 = Math::mix( p1[ 0 ], p1[ 1 ], mix.x );
                return Math::mix( v0, v1, mix.y );
            }

    };

    template <typename T>
    inline RGBDForwardAlignment<T>::RGBDForwardAlignment( const Matrix3<T> & intrinsics ) :
        _kx( IKernel::HAAR_HORIZONTAL_3 ),
        _ky( IKernel::HAAR_VERTICAL_3 ),
        _gaussX( IKernel::GAUSS_HORIZONTAL_3 ),
        _gaussY( IKernel::GAUSS_VERTICAL_3 ),
        _currentImagePyramid( _params.octaves, _params.octaveScale ),
        _initial( true )
    {
        _kx.scale( -0.5f );
        _ky.scale( -0.5f );

        _calibForScale.resize( _params.octaves );
        _calibForScale[ 0 ] = intrinsics;
        for( size_t i = 1; i < _params.octaves; i++ ){
            _calibForScale[ i ] = _calibForScale[ i - 1 ] * _params.octaveScale;
            _calibForScale[ i ][ 2 ][ 2 ] = 1.0;
        }

        _templateForScale.resize( _params.octaves );
    }

    template <typename T>
    inline RGBDForwardAlignment<T>::~RGBDForwardAlignment()
    {
    }

    template <typename T>
    inline void RGBDForwardAlignment<T>::updateTemplateData( const Image& depth, const Matrix4<T>& pose )
    {
        for( size_t i = 0; i < _currentImagePyramid.octaves(); i++ ){
            _templateForScale[ i ].updateTemplate( _currentImagePyramid[ i ], depth, pose,  _calibForScale[ i ], _params.templateParams );
        }
    }

    template <typename T>
    inline void RGBDForwardAlignment<T>::alignFrames( Matrix4<T>& pose, const Image& gray, const Image& depth )
    {
        // update the pyramid:
        _currentImagePyramid.update( gray );

        if( _initial ){
            updateTemplateData( depth, pose );
            _initial = false;
            return;
        }

        //Matrix4<T> poseEstimate = pose.inverse();
        //Eigen::Matrix<T, 4, 4> pE;
        //EigenBridge::toEigen( pE, poseEstimate );

        Matrix4<T> poseEstimate;
        poseEstimate.setIdentity();
        SE3<T> poseRep;

        // TODO: relative pose! We start at identity
        //poseRep.set( pE );

        typename SE3<T>::ScreenJacType sJ;
        typename SE3<T>::PointType point3DEigen;
        typename SE3<T>::ParameterVectorType delta;
        Eigen::Matrix<T, 3, 3> KEigen;
        Eigen::Matrix<T, 1, 6> j, jSum;
        Eigen::Matrix<T, 6, 6> jtjSum;
        Eigen::Matrix<T, 1, 2> grad;

        // for each octave:
        Image gx, gy;
        Vector2f interpolationWeights;
        for( int o = _currentImagePyramid.octaves() - 1; o >= 0; o-- ){
            // compute the gradients:
            const Image& currI = _currentImagePyramid[ o ];
            const RGBDTemplate<T>& tData = _templateForScale[ o ];
            const float* templatePixels = tData.pixels();
            computeImageGradients( gx, gy, currI );

            // optimization on this scale:
            size_t iter = 0;
            std::vector<Vector2<T> > pts2d;
            std::vector<Vector3<T> > pts3d;
            pts2d.resize( tData.numPoints() );
            pts3d.resize( tData.numPoints() );

            size_t w = currI.width() - 1;
            size_t h = currI.height() - 1;

            EigenBridge::toEigen( KEigen, _calibForScale[ o ] );

            IMapScoped<const float> gxMap( gx );
            IMapScoped<const float> gyMap( gy );
            IMapScoped<const float> iMap( currI );
            size_t floatStride = iMap.stride() / sizeof( float );

            while( iter < _params.maxIterationsPerScale ){
                // warp the template points to image space using K, R & t
                transformPoints( &pts3d[ 0 ], &pts2d[ 0 ], tData.points(),
                                 poseEstimate, _calibForScale[ o ], tData.numPoints() );

                jSum.setZero();
                jtjSum.setZero();

                // for each point that projects into the image:
                for( size_t i = 0; i < pts2d.size(); i++ ){
                    const Vector2<T>& p2 = pts2d[ i ];
                    if( pts3d[ i ].z > 0 && p2.x >= 0.0 && p2.x < w && p2.y >= 0 && p2.y < h ){

                        EigenBridge::toEigen( point3DEigen, pts3d[ i ] );
                        poseRep.screenJacobian( sJ, point3DEigen, KEigen );

                        int x = p2.x;
                        int y = p2.y;
                        size_t offset0 = y * floatStride + x;
                        size_t offset1 = offset0 + floatStride;

                        computeMixingCoeffs( interpolationWeights, p2 );

                        // interpolate the gradient & the pixel value:
                        grad( 0, 0 ) = interpolatePixelValue( gxMap.ptr() + offset0,
                                                              gxMap.ptr() + offset1,
                                                              interpolationWeights );
                        grad( 0, 1 ) = interpolatePixelValue( gyMap.ptr() + offset0,
                                                              gyMap.ptr() + offset1,
                                                              interpolationWeights );

                        float v = interpolatePixelValue( iMap.ptr() + offset0,
                                                         iMap.ptr() + offset1,
                                                         interpolationWeights );

                        // the final jacobian
                        j = grad * sJ;
                        jtjSum += ( j.transpose() * j );
                        jSum   += ( j * ( v -  templatePixels[ i ] ) );

                    }
                }

                delta = -jtjSum.inverse() * jSum.transpose();
                if( delta.norm() < _params.minParamUpdate )
                    break;

                poseRep.apply( delta );

                EigenBridge::toCVT( poseEstimate, poseRep.transformation() );

                iter++;
            }
        }

        pose = pose * poseEstimate.inverse();

        updateTemplateData( depth, pose );
    }

    template <typename T>
    inline void RGBDForwardAlignment<T>::transformPoints( Vector3<T>* warped3d,
                                                   Vector2<T>* warped2d,
                                                   const Vector3<T>* pts3d,
                                                   const Matrix4<T> & mat,
                                                   const Matrix3<T> K,
                                                   size_t n )
    {
        for( size_t i = 0; i < n; i++ ){
            warped3d[ i ] = mat * pts3d[ i ];
            warped2d[ i ] = K * warped3d[ i ];
        }
    }


}

#endif // RGBDFORWARDALIGNMENT_H
