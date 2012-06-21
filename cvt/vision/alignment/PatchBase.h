/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_PATCH_BASE_H
#define CVT_PATCH_BASE_H

#include <cvt/gfx/IMapScoped.h>
#include <Eigen/Dense>
#include <Eigen/StdVector>

namespace cvt
{
    struct AlignInfo
    {
        size_t numPixels;
        float  ssd;
        float  sad;
        size_t iterations;
    };

    template <typename T>
    struct PatchTraits;

    template <class DerivedPatch>
    class PatchBase
    {
        public:
            typedef PatchTraits<DerivedPatch> Trait;
            typedef typename Trait::PType    PoseType;
            typedef typename Trait::HessType HessType;
            typedef typename Trait::JacType  JacType;

            PatchBase()
            {
            }

            bool alignWithImage( const Image& img )
            {
                return static_cast<DerivedPatch*>( this )->alignWithImage( img );
            }

            const AlignInfo& alignInfo() const { return _lastAlignment; }

            void currentCenter( Vector2f& center )  const
            {
                static_cast<DerivedPatch*>( this )->currentCenter( center );
            }

            PoseType&	pose()
            {
                return static_cast<DerivedPatch*>( this )->pose();
            }

            void initPose( const Vector2f& pos )
            {
                static_cast<DerivedPatch*>( this )->initPose( pos );
            }

            const uint8_t*   pixels()      const { return _pixelData; }
            const uint8_t*   transformed() const { return _lastTransformed; }

            const HessType&  inverseHessian() const { return _inverseHessian; }
            const JacType*   jacobians()      const { return _jacobians; }

            bool update( const uint8_t* imgPtr, size_t stride, const Vector2f& pos )
            {
                static const float pHalf = ( DerivedPatch::PatchSize >> 1 );
                const uint8_t* ptr = imgPtr + ( int )( pos.y - pHalf ) * stride + ( int )( pos.x - pHalf );
                const uint8_t* nextLine = ptr + stride;
                const uint8_t* prevLine = ptr - stride;

                size_t numLines = DerivedPatch::PatchSize;

                JacType* J = _jacobians;
                HessType& invH = _inverseHessian;
                uint8_t* p = _pixelData;
                uint8_t* t = _lastTransformed;

                Eigen::Matrix<float, 2, 1> g;
                HessType hess( HessType::Zero() );
                typename PoseType::ScreenJacType sj;

                Eigen::Vector2f point( -pHalf, -pHalf );
                while( numLines-- )
                {
                    point[ 0 ] = -pHalf;
                    for( size_t i = 0; i < DerivedPatch::PatchSize; i++ ){
                        *p = *t = ptr[ i ];

                        // sobel style
                        g[ 0 ] = -( int16_t )prevLine[ i - 1 ] + ( int16_t )prevLine[ i + 1 ]
                                 + 2 * ( -( int16_t )ptr[ i - 1 ] + ( int16_t )ptr[ i + 1 ] )
                                 -( int16_t )nextLine[ i - 1 ] + ( int16_t )nextLine[ i + 1 ];

                        g[ 1 ] = ( int16_t )nextLine[ i - 1 ] + 2 * ( int16_t )nextLine[ i ] + ( int16_t )nextLine[ i + 1 ]
                                -( int16_t )prevLine[ i - 1 ] - 2 * ( int16_t )prevLine[ i ] - ( int16_t )prevLine[ i + 1 ];
                        //g *= 0.125f;
                        g *= 0.25f;

                        // simple central derivative
                        //g[ 0 ] = ( int16_t )ptr[ i + 1 ] - ( int16_t )ptr[ i - 1 ];
                        //g[ 1 ] = ( int16_t )nextLine[ i ] - ( int16_t )prevLine[ i ];
                        //g *= 0.5f;

                        // in inverse comp. the sj is the same for each single patch
                        // it could be precomputed and stored once statically?!
                        _pose.screenJacobian( sj, point );
                        *J =  sj.transpose() * g;

                        hess += ( *J ) * J->transpose();
                        J++;
                        p++;
                        t++;

                        point[ 0 ] += 1.0f;
                    }

                    prevLine = ptr;
                    ptr = nextLine;
                    nextLine += stride;
                    point[ 1 ] += 1.0f;
                }

                initPose( pos );

                float det = hess.determinant();
                if( Math::abs( det ) > 1e-6 ){
                    invH = hess.inverse();
                    return true;
                }

                return false;
            }

        private:
            PatchBase( const PatchBase& );
            PatchBase& operator= (const PatchBase& );

        protected:
            uint8_t     _pixelData[ Trait::PatchSize * Trait::PatchSize ];
            uint8_t     _lastTransformed[ Trait::PatchSize * Trait::PatchSize ];
            HessType    _inverseHessian;
            JacType     _jacobians[ Trait::PatchSize * Trait::PatchSize ];
            PoseType    _pose;

            float buildSystem( JacType& jacSum,
                               const Matrix3f& pose,
                               const uint8_t* imgPtr, size_t iStride )
            {
                // the warped current ones
                uint8_t* warped = _lastTransformed;

                // the original template pixels
                const uint8_t* temp = _pixelData;
                const JacType* J = _jacobians;

                static const float half = DerivedPatch::PatchSize >> 1;
                float diffSum = 0.0f;

                for( size_t i = 0; i < 3; i++ ){
                    for( size_t k = 0; k < 3; k++ ){
                        if( Math::isNaN( pose[ i ][ k ] ) ){
                            std::cout << "Matrix is NAN!" << std::endl;
                            return Math::MAXF;
                        }
                    }
                }

                Vector2f pcur, ppcur;
                pcur.y = -half;
                for( size_t y = 0; y < DerivedPatch::PatchSize; y++ ){
                    pcur.x = -half;
                    for( size_t i = 0; i < DerivedPatch::PatchSize; i++ ){
                        ppcur = pose * pcur;

                        int ix = ( int )ppcur.x;
                        int iy = ( int )ppcur.y;
                        float fracx = ppcur.x - ix;
                        float fracy = ppcur.y - iy;

                        const uint8_t* px = imgPtr + iy * iStride + ix;
                        uint8_t v0 = Math::mix( px[ 0 ], px[ 1 ], fracx );
                        px += iStride;

                        uint8_t v1 = Math::mix( px[ 0 ], px[ 1 ], fracx );
                        v0 = Math::mix( v0, v1, fracy );
                        warped[ y * DerivedPatch::PatchSize + i ] = v0;

                        float deltaImg = ( int16_t )v0 - ( int16_t )temp[ y * DerivedPatch::PatchSize + i ];
                        diffSum += Math::sqr( deltaImg );
                        jacSum += ( *J *  deltaImg );
                        J++;
                        pcur.x += 1.0f;
                    }
                    pcur.y += 1.0f;
                }
                return diffSum;
            }

            bool patchIsInImage( const Matrix3f& pose, size_t w, size_t h ) const
            {
                static const float half = DerivedPatch::PatchSize >> 1;
                static const Vector2f a( -half, -half );
                static const Vector2f b(  half, -half );
                static const Vector2f c(  half,  half );
                static const Vector2f d( -half,  half );

                Vector2f pWarped;

                pWarped = pose * a;
                if( pWarped.x < 0.0f || pWarped.x >= w ||
                    pWarped.y < 0.0f || pWarped.y >= h )
                    return false;

                pWarped = pose * b;
                if( pWarped.x < 0.0f || pWarped.x >= w ||
                    pWarped.y < 0.0f || pWarped.y >= h )
                    return false;

                pWarped = pose * c;
                if( pWarped.x < 0.0f || pWarped.x >= w ||
                    pWarped.y < 0.0f || pWarped.y >= h )
                    return false;

                pWarped = pose * d;
                if( pWarped.x < 0.0f || pWarped.x >= w ||
                    pWarped.y < 0.0f || pWarped.y >= h )
                    return false;
                return true;
            }

        private:
            AlignInfo _lastAlignment;
    };

}

#endif
