/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_KLT_TRACKER_H
#define CVT_KLT_TRACKER_H

#include <cvt/math/Vector.h>
#include <cvt/geom/Line2D.h>
#include <cvt/gfx/Image.h>
#include <vector>

#include <cvt/vision/KLTPatch.h>
#include <cvt/math/Translation2D.h>
#include <cvt/math/Math.h>
#include <cvt/util/EigenBridge.h>


namespace cvt
{
	template <class PoseType, size_t pSize=16>
	class KLTTracker
	{
		public:
			typedef KLTPatch<pSize, PoseType>	KLTPType;

			KLTTracker( size_t maxIters = 20 );

			float ssdThreshold() const { return _ssdThresh; }
			void  setSSDThreshold( float ssd ) { _ssdThresh = ssd; }

			/* percentage of pixels that need to project in current view */
			void  setEarlyStepOutAvgSSD( float ssd ){ _earlyStepOutThresh = ssd; }
			
			bool trackPatch( KLTPType& patch,
							 const uint8_t* current, size_t currStride,
							 size_t width, size_t height );

		private:
			size_t _maxIters;
			float  _ssdThresh;
			float  _earlyStepOutThresh;
	};


	template <class PoseType, size_t pSize>
	inline KLTTracker<PoseType, pSize>::KLTTracker( size_t maxIters ) :
		_maxIters( maxIters ),
		_ssdThresh( 0.0f )
	{
		setEarlyStepOutAvgSSD( 10.0f );
		_ssdThresh = Math::sqr( 20.0f );
	}

	template <class PoseType, size_t pSize>
	inline bool KLTTracker<PoseType, pSize>::trackPatch( KLTPType& patch,
									   					 const uint8_t* current, size_t currStride,
									   					 size_t width, size_t height )
	{

		size_t iter = 0;

		Vector2f point;
		typename KLTPType::JacType jSum;
		typename PoseType::ParameterVectorType delta;
		float diffSum = 0.0f;
		size_t npix = 0;

        // normal on x
		Vector3f nx( 1.0f, 0.0f, 0.0f );
        
        Vector2f a( 0.0f, 0.0f );
        Vector2f b( pSize, 0.0f );
        Vector2f c( pSize, pSize );
        Vector2f d( 0.0f, pSize );
        Vector2f pWarped;
        
		while( iter < _maxIters ){
			jSum.setZero();
			
			diffSum = 0.0f;
			npix = 0;

			//pose matrix
			Matrix3f pose;
			EigenBridge::toCVT( pose, patch.pose().transformation() );
            
            // first test if all points transform into the image, 
            // if not, just return false -> patch no longer tracked
            pWarped = pose * a;
            if( pWarped.x < 0.0f || pWarped.x > ( width-1 ) ||
                pWarped.y < 0.0f || pWarped.y > ( height-1 ) )
                return false;
            pWarped = pose * b;
            if( pWarped.x < 0.0f || pWarped.x > ( width-1 ) ||
               pWarped.y < 0.0f || pWarped.y > ( height-1 ) )
                return false;

            pWarped = pose * c;
            if( pWarped.x < 0.0f || pWarped.x > ( width-1 ) ||
               pWarped.y < 0.0f || pWarped.y > ( height-1 ) )
                return false;
            pWarped = pose * d;
            if( pWarped.x < 0.0f || pWarped.x > ( width-1 ) ||
               pWarped.y < 0.0f || pWarped.y > ( height-1 ) )
                return false;
            
            // the warped current ones
            uint8_t* warped = patch.transformed();
            // the original template pixels
            const uint8_t* temp = patch.pixels();
            const typename KLTPType::JacType* J = patch.jacobians();

			Vector2f pcur, ppcur;
			for( size_t y = 0; y < pSize; y++ ){
                for( size_t i = 0; i < pSize; i++ ){
					pcur.y = y;
					pcur.x = i;
					ppcur = pose * pcur;

					if( Math::isNaN( ppcur.x ) || Math::isNaN( ppcur.y ) ){
						std::cout << "ISNAN" << std::endl;
						return false;
					}

					int ix = ( int )ppcur.x;
					int iy = ( int )ppcur.y;
					float fracx = ppcur.x - ix;
					float fracy = ppcur.y - iy;

					const uint8_t* px = current + iy * currStride + ix;
					uint8_t v0 = Math::mix( px[ 0 ], px[ 1 ], fracx );
					px += currStride;
					uint8_t v1 = Math::mix( px[ 0 ], px[ 1 ], fracx );
					v0 = Math::mix( v0, v1, fracy );
					warped[ y * pSize + i ] = v0; 
					
					float deltaImg = ( int16_t )v0 - ( int16_t )temp[ y * pSize + i ];
                    diffSum += Math::sqr( deltaImg * deltaImg );
                    jSum += ( *J *  deltaImg );
                    J++;
				
				}
                
			}

			// solve for the delta:
			delta = patch.inverseHessian() * jSum;
			patch.pose().applyInverse( -delta );

			/* early step out? */
			//if( ( diffSum / Math::sqr( pSize ) ) < _earlyStepOutThresh )
			//	return true;
			
			iter++;
		}

		if( ( diffSum / Math::sqr<float>( pSize ) ) > _ssdThresh ){
			return false;
		}
		return true;
	}

}

#endif
