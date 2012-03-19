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
#include <cvt/util/EigenBridge.h>

namespace cvt
{
	template <class PoseType, size_t pSize=16>
	class KLTTracker
	{
		public:
			typedef KLTPatch<pSize, PoseType>	KLTPType;

			KLTTracker( size_t maxIters = 20 );

			void trackFeatures( std::vector<size_t> & trackedIndices,
							    const std::vector<KLTPType*>& patches,
							    const Image& current );

			float ssdThreshold() const { return _ssdThresh; }
			void  setSSDThreshold( float ssd ) { _ssdThresh = ssd; }

			/* percentage of pixels that need to project in current view */
			void  setMinPixFraction( float frac ){ _minPix = frac * Math::sqr( pSize ); }
			void  setEarlyStepOutAvgSSD( float ssd ){ _earlyStepOutThresh = ssd; }
			
			bool trackPatch( KLTPType& patch,
							 const uint8_t* current, size_t currStride,
							 size_t width, size_t height );

		private:
			size_t _maxIters;
			float  _ssdThresh;
			size_t _minPix;
			float  _earlyStepOutThresh;
			
			bool checkBounds( const Eigen::Vector2f & p, size_t width, size_t height ) const
			{
				size_t h = pSize >> 1;
				if( p[ 0 ] <= h || p[ 1 ] <= h || p[ 0 ] + h >= width || p[ 1 ] + h >= height )
					return false;
				return true;
			}

	};


	template <class PoseType, size_t pSize>
	inline KLTTracker<PoseType, pSize>::KLTTracker( size_t maxIters ) :
		_maxIters( maxIters ),
		_ssdThresh( 0.0f )
	{
		setMinPixFraction( 0.9f );
		setEarlyStepOutAvgSSD( 10.0f );
		_ssdThresh = Math::sqr( 20.0f );
	}

	template <class PoseType, size_t pSize>
	inline void KLTTracker<PoseType, pSize>::trackFeatures( std::vector<size_t> & trackedIndices,
									const std::vector<KLTPType*>& patches,
									const Image& current )
	{
		size_t currStride;
		const uint8_t* currImgPtr = current.map( &currStride );

		size_t w = current.width();
		size_t h = current.height();	

		Eigen::Vector2f pp;
		Eigen::Vector2f p2( 0.0f, 0.0f );
		for( size_t i = 0; i < patches.size(); i++ ){
			// track each single feature
			const KLTPType& patch = *patches[ i ];
		   	patch.pose().transform( pp, p2 );
			if( !checkBounds( pp, w, h ) ){
				continue;
			}

			if( trackPatch( patch, currImgPtr, currStride, w, h ) ){
				trackedIndices.push_back( i );
			}
		}

		current.unmap( currImgPtr );
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

		Line2Df scanLine( Vector3f( 0.0f, 1.0f, 0.0f ) );
		Line2Df lWarped;

		const typename KLTPType::JacType* J = patch.jacobians();
		const uint8_t* temp = patch.pixels();

		uint8_t	warpedLine[ pSize ];
		float	positions[ 2 * pSize ];
		while( iter < _maxIters ){
			jSum.setZero();
			
			diffSum = 0.0f;
			npix = 0;

			//Matrix3f poseT = patch.pose().transformation().transpose();

			for( size_t y = 0; y < pSize; y++ ){
				//scanLine[ 2 ] = -y; 
				//lWarped = poseT * scanLine.vector();

				//if( Clipping::clip( r, l2, pt1, pt2 ) ) {
				//	Vector2f px1, px2;
				//	px1 = T * pt1;
				//	px2 = T * pt2;

				//	if( px1.x > px2.x ) {
				//		Vector2f tmp = px1;
				//		px1 = px2;
				//		px2 = tmp;
				//	}

				//	ssize_t x1 =  Math::clamp<ssize_t>( px1.x, 0, w );
				//	ssize_t len =  Math::clamp<ssize_t>( px2.x + 1, 0, w ) -x1;
				//	Vector3f p = Tinv * Vector3f( x1, y, 1.0f );
				//	simd->warpLinePerspectiveBilinear1u8( dst.ptr() + x1, src.ptr(), src.stride(), sw, sh,
				//										 p.ptr(), nx.ptr(), len );
				//}

				/*
				if( ( size_t )pp[ 0 ] < width && ( size_t )pp[ 1 ] < height ){
					float deltaImg = ( int16_t )current[ ( size_t )pp[ 1 ] * currStride + ( size_t )pp[ 0 ] ] - ( int16_t )*temp;
					diffSum += ( deltaImg * deltaImg );
					jSum += ( *J *  deltaImg );
					npix++;
				}

					temp++;
					J++;
					p2[ 0 ] += 1;
				}
				*/
			}
			
			if( npix < _minPix ){
				return false;
			}

			// solve for the delta:
			delta = patch.inverseHessian() * jSum;

			patch.pose().applyInverse( -delta );

			/* early step out? */
			if( ( diffSum / npix ) < _earlyStepOutThresh )
				return true;
			
			iter++;
		}

		if( ( diffSum / npix ) > _ssdThresh ){
			return false;
		}
		return true;
	}

}

#endif
