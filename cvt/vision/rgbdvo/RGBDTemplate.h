/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef CVT_RGBDTEMPLATE_H
#define CVT_RGBDTEMPLATE_H

#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/EigenBridge.h>

#include <Eigen/Core>

namespace cvt {

    template <typename T>
    class RGBDTemplate
    {
        public:
            struct Params
            {
                Params() :
                    depthScale( 1.031 * 5000.0 ),
                    minDepth( 0.2 )
                {
                }

                T depthScale;
                T minDepth;
            };

            typedef cvt::Vector3<T>         WorldPointType;
            typedef cvt::Matrix4<T>			Matrix4Type;
            typedef cvt::Matrix3<T>			Matrix3Type;

            // Todo: This is Model Dependent!
            // e.g. if we use affine illumination or less parameters
            typedef Eigen::Matrix<T, 6, 1>  JacType;

            RGBDTemplate()
            {}

            ~RGBDTemplate()
            {}

            /* extract 3D Points from depth & corresponding pixel values */
            void updateTemplate( const Image& gray, const Image& depth, const Matrix4<T>& pose, const Matrix3Type& K, const Params &params );

            const WorldPointType*   points()    const { return &_points3d[ 0 ]; }            
            const float*            pixels()    const { return &_pixelValues[ 0 ]; }
            size_t                  numPoints() const { return _points3d.size(); }

        private:
            std::vector<float>          _pixelValues;
            std::vector<Vector3<T> >    _points3d;

            // offsets into the mapped floating point image
            std::vector<size_t>         _pointOffsets;


    };

    template <typename T>
    inline void RGBDTemplate<T>::updateTemplate( const Image& gray, const Image& depth, const Matrix4<T>& pose, const Matrix3Type& K, const Params &params )
    {
        T depthScaling = ( T )0xffff / params.depthScale;

        // we have to take into account the scale factor
        T scale = ( float )depth.width() / ( float )gray.width();

        T invFx = 1.0f / K[ 0 ][ 0 ];
        T invFy = 1.0f / K[ 1 ][ 1 ];
        T cx    = K[ 0 ][ 2 ];
        T cy    = K[ 1 ][ 2 ];

        // temp vals
        std::vector<T> tmpx( gray.width() );
        std::vector<T> tmpy( gray.height() );

        for( size_t i = 0; i < tmpx.size(); i++ ){
            tmpx[ i ] = ( i - cx ) * invFx;
        }
        for( size_t i = 0; i < tmpy.size(); i++ ){
            tmpy[ i ] = ( i - cy ) * invFy;
        }

        IMapScoped<const float> grayMap( gray );
        IMapScoped<const float> depthMap( depth );

        Eigen::Matrix3f Keigen;
        EigenBridge::toEigen( Keigen, K );

        size_t floatStride = grayMap.stride() / sizeof( float );

        { // prereserve space
            const size_t reserveSize = gray.width() * gray.height() * 0.8;
            _pixelValues.reserve( reserveSize );
            _pointOffsets.reserve( reserveSize );
            _points3d.reserve( reserveSize );
        }

        for( size_t y = 0; y < gray.height(); y++ ){
            const float* value = grayMap.ptr();

            depthMap.setLine( scale * y );
            const float* d = depthMap.ptr();
            for( size_t x = 0; x < gray.width(); x++ ){
                T z = d[ ( size_t ) scale * x ] * depthScaling;
                if( z > params.minDepth ){
                    _pixelValues.push_back( value[ x ] );
                    _pointOffsets.push_back( floatStride * y + x );
                    Vector3f p( tmpx[ x ] * z, tmpy[ y ] * z, z );
                    _points3d.push_back( pose * p );



                    std::cout << "Added Point:  " << p << std::endl;
                    std::cout << "Scale: " << scale << std::endl;
                    //std::cout << "World Coords: " << _points3d.back() << std::endl;

                }
            }
            grayMap++;
        }
	}

}

#endif // RGBDTEMPLATE_H
