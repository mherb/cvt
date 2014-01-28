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


#ifndef CVT_RGBDPREPROCESSOR_H
#define CVT_RGBDPREPROCESSOR_H

#include <cvt/gfx/IKernel.h>
#include <cvt/gfx/Image.h>
#include <cvt/vision/ImagePyramid.h>

namespace cvt {
    class RGBDPreprocessor {
        public:
            static RGBDPreprocessor& instance()
            {
                static RGBDPreprocessor _instance;
                return _instance;
            }

            ~RGBDPreprocessor()
            {}

            void gradient( Image& gx, Image& gy, const Image& src )
            {
                gx.reallocate( src.width(), src.height(), IFormat::GRAY_FLOAT );
                gy.reallocate( src.width(), src.height(), IFormat::GRAY_FLOAT );
                src.convolve( gx, _dx );
                src.convolve( gy, _dy );
            }

            void gradient( ImagePyramid& gx, ImagePyramid& gy, const ImagePyramid& src )
            {
                src.convolve( gx, _dx );
                src.convolve( gy, _dy );
            }

            void setGradientKernels( const IKernel& dx, const IKernel& dy )
            {
                _dx = dx;
                _dy = dy;
            }

            const IKernel& dxKernel() const { return _dx; }
            const IKernel& dyKernel() const { return _dy; }

            // TODO: handle this more nicely: the problem is, that the Image has type float and is normalized between 0.0f-1.0f
            // for uint32_t the max value is 0xFFFF, we want to convert to meters, therefore we need to define the scaling
            // #val pixvals corresponds to 1m --> scale by (uint16_t max) * 1/val
            void setDepthScale( float scale ) { _depthScaling = ( float )( 0xFFFF ) / scale; }
            void setMinDepth( float minDepth ){ _minDepth = minDepth; }
            void setMaxDepth( float maxDepth ){ _maxDepth = maxDepth; }

            float interpolateDepth( const Vector2f& p,
                                    const float* ptr, size_t stride ) const
            {
                // get the fractions:
                int xi = p.x;
                int yi = p.y;

                const float* curr = ptr + yi * stride + xi ;

                float wx = p.x - xi;
                float wy = p.y - yi;

                float z0 = 0.0f;
                if( curr[ 0 ] > 0.0f ){
                    if( curr[ 1 ] > 0.0f ){
                        // both values valid
                        z0 = Math::mix( curr[ 0 ], curr[ 1 ], wx );
                    }  else {
                        z0 = curr[ 0 ];
                    }
                } else {
                    if( curr[ 1 ] ){
                        z0 = curr[ 1 ];
                    } else {
                        // no value valid in this line
                        wy = 1.0f;
                    }
                }

                float z1 = 0.0f;
                if( curr[ stride ] > 0.0f ){
                    if( curr[ stride + 1 ] > 0.0f ){
                        // both values valid
                        z1 = Math::mix( curr[ stride ], curr[ stride + 1 ], wx );
                    }  else {
                        z1 = curr[ stride ];
                    }
                } else {
                    if( curr[ stride + 1 ] > 0.0f ){
                        z1 = curr[ stride + 1 ];
                    } else {
                        // no value valid in this line
                        wy = 0.0f;
                    }
                }
                float zall = Math::mix( z0, z1, wy ) * _depthScaling;
                if( zall > _minDepth && zall < _maxDepth )
                    return zall;
                return -1.0f;
            }

        private:
            RGBDPreprocessor() :
                _dx( IKernel::FIVEPOINT_DERIVATIVE_HORIZONTAL ),
                _dy( IKernel::FIVEPOINT_DERIVATIVE_VERTICAL ),
                _depthScaling( 1.0f ),
                _minDepth( 0.05f ),
                _maxDepth( 10.0f )
            {
                float s = -1.0f;
                _dx.scale( s );
                _dy.scale( s );
            }

            /* don't allow copies */
            RGBDPreprocessor( const RGBDPreprocessor& );

            IKernel _dx;
            IKernel _dy;

            /* params for unprojection */
            float   _depthScaling;
            float   _minDepth;
            float   _maxDepth;

    };

}

#endif // RGBDPREPROCESSOR_H
