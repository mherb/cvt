#include "FeatureTracker.h"

#include <cvt/geom/KDTree.h>
#include <cvt/vision/AGAST.h>
#include <cvt/vision/FAST.h>
#include <cvt/math/Math.h>

#include <set>

namespace cvt {

	FeatureTracker::FeatureTracker() :
		_featureDetector( 0 )
	{
		_featureDetector = new AGAST( OAST_9_16 );
		//_featureDetector = new FAST( SEGMENT_9 );
	}

	FeatureTracker::~FeatureTracker()
	{
		if( _featureDetector )
			delete _featureDetector;
	}

	void FeatureTracker::run( const Image & current,
							  const Image & last,
							  const std::vector<Feature2D> & predicted,
							  std::vector<Feature2D> & trackedPoints,
							  std::vector<size_t> & indices,
							  std::vector<Feature2D> & newFeatures )
	{
		std::vector<Feature2D> newPoints;

		//_featureDetector->extract( current, newPoints );
		_featureDetector->extractMultiScale( current, newPoints, 3 );

		if( newPoints.size() ){
			KDTree<Feature2D> kd( newPoints );

			std::set<uint32_t> matchedIndices;

			// find nearest neighbor for each point
			for( size_t i = 0; i < predicted.size(); i++ ){
				// locate within 7x7 pixel distance
				uint32_t index = kd.locate( predicted[ i ], 5.0f );

				// prune too far points
				if( ( newPoints[ index ] - predicted[ i ] ).length() < 5.0 ){
					float dist = patchDistance( current, last, newPoints[ index ], predicted[ i ] );
					if( dist < 20.0f ){
						trackedPoints.push_back( newPoints[ index ] );
						indices.push_back( index );
						matchedIndices.insert( index );
					}
				}
			}

			for( uint32_t i = 0; i < newPoints.size(); i++ ){
				if( matchedIndices.find( i ) == matchedIndices.end() ){
					newFeatures.push_back( newPoints[ i ] );
				}
			}
		}
	}
	
	float FeatureTracker::patchDistance( const Image & im0, const Image & im1,
									     const Feature2D & pos0, const Feature2D & pos1 )
	{
		size_t stride0, stride1;
		const uint8_t * i0 = im0.map( &stride0 );
		const uint8_t * i1 = im1.map( &stride1 );

		/* 7x7 Offset */
		int32_t h = 7;
		int32_t xOffset = h >> 1;
		int32_t yOffset = h >> 1;

		// border check: TODO: we don't need to 
		if( pos0[ 0 ] < xOffset || pos0[ 0 ] > (int32_t)( im0.width() - xOffset ) ||
		 	pos0[ 1 ] < yOffset || pos0[ 1 ] > (int32_t)( im0.height() - yOffset ) ||
			pos1[ 0 ] < xOffset || pos1[ 0 ] > (int32_t)( im1.width() - xOffset ) ||
			pos1[ 1 ] < yOffset || pos1[ 1 ] > (int32_t)( im1.height() - yOffset ) )
			return 100.0f;

		const uint8_t * p0 = i0 + stride0 * ( pos0[ 1 ] - yOffset ) + pos0[ 0 ] - xOffset;
		const uint8_t * p1 = i1 + stride1 * ( pos1[ 1 ] - yOffset ) + pos1[ 0 ] - xOffset;

		int32_t hIter = h;
		float distance = 0.0;
		while( hIter-- ){
			for( int32_t i = 0; i < h; i++ ){
				distance +=  Math::sqr( ( float )p0[ i ] - ( float )p1[ i ] );
				//distance +=  Math::abs( ( float )p0[ i ] - ( float )p1[ i ] );
			}
			p0 += stride0;
			p1 += stride1;
		}

		im0.unmap( i0 );
		im1.unmap( i1 );

		return distance / Math::sqr( 7 );
	}

}
