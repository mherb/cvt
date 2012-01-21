#ifndef CVT_FEATURE_TRACKING_H
#define CVT_FEATURE_TRACKING_H

#include <vector>
#include <cvt/vision/FeatureMatch.h>
#include <cvt/geom/PointSet.h>

namespace cvt
{
	class FeatureTracking
	{
		public:
			virtual ~FeatureTracking() {}

			virtual void trackFeatures( PointSet3d&			p3d, 
										PointSet2d&			p2d,
										const SlamMap&		map,
										const SE3<double>&	pose,
										const Image&		img ) = 0;

			virtual void clear() = 0;


			virtual size_t triangulateNewFeatures( SlamMap& map, 
												   const SE3<double>& pose, 
												   const Image& first, 
												   const Image& second ) = 0;

		protected:

			/* triangulate a new point */	
			virtual float triangulateSinglePoint( Vector4f&			newPoint, 
												  const Vector2f&	point0,
												  const Vector2f&	point1,
												  const Matrix4f&	projMat0,
												  const Matrix4f&	projMat1 ) const;

			inline bool checkFeatureSAD( const Vector2f& p0, const Vector2f& p1, const Image & i0, const Image & i1 ) const;
	};


	inline float FeatureTracking::triangulateSinglePoint( Vector4f&			newPoint, 
														  const Vector2f&	point0,
														  const Vector2f& 	point1,
														  const Matrix4f& 	projMat0,
														  const Matrix4f& 	projMat1 ) const
	{
		Vector4f repr;
		Vector2f repr2;

		Vision::triangulate( newPoint, projMat0, projMat1, point0, point1 );
			
		// normalize 4th coord;
		newPoint /= newPoint.w;
		float error = 10000.0f;
		if( newPoint.z > 0.0f && newPoint.z < 20 ){
			repr = projMat0 * newPoint;
			repr2.x = repr.x / repr.z;
			repr2.y = repr.y / repr.z;
			error = ( point0 - repr2 ).length();

			repr = projMat1 * newPoint;
			repr2.x = repr.x / repr.z;
			repr2.y = repr.y / repr.z;
			error += ( point1 - repr2 ).length();

			error /= 2.0f;
		} 
		
		return error;
	}

	inline bool FeatureTracking::checkFeatureSAD( const Vector2f& point0, const Vector2f& point1, const Image & i0, const Image & i1 ) const
	{
		size_t s0, s1;
		const uint8_t* ptr0 = i0.map( &s0 );
		const uint8_t* ptr1 = i1.map( &s1 );

		const uint8_t* p0 = ptr0 + ( (int)point0.y - 8 ) * s0 + ( (int)point0.x - 8 );
		const uint8_t* p1 = ptr1 + ( (int)point1.y - 8 ) * s1 + ( (int)point1.x - 8 );

		float sad = 0;
		for( size_t i = 0; i < 16; i++ ){	
			sad += SIMD::instance()->SAD( p0, p1, 16 );
			p0 += s0;
			p1 += s1;
		}

		i0.unmap( ptr0 );
		i1.unmap( ptr1 );

		// average & normalize
		sad = 1.0f - ( sad / Math::sqr( 256.0 ) );
	
		return ( sad > 0.7f ) ? true : false;	
	}
}

#endif
