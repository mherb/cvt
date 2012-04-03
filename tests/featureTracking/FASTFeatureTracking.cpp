#include "FASTFeatureTracking.h"

namespace cvt
{
	FASTFeatureTracking::FASTFeatureTracking() :
		_detector( SEGMENT_9 ),
		_pyramid( 3, 0.5f )
	{
		_detector.setNonMaxSuppress( true );
		_detector.setThreshold( 12 );
	}

	FASTFeatureTracking::~FASTFeatureTracking()
	{
	}

	void FASTFeatureTracking::trackFeatures( std::vector<PatchType*>& tracked,
											 std::vector<PatchType*>& predicted,
											 const Image& image )
	{
		std::vector<Feature2Df> currentFeatures;

		// detect current fast features
		_associatedIndexes.clear();
		_currentFeatures.clear();
		detectCurrentFeatures( image );

		for( size_t i = 0; i < predicted.size(); i++ ){
			PatchType* patch = predicted[ i ];

			// find best fast feature within a certain window
			int idx = bestFASTFeatureInRadius( image, *patch );

			if( idx != -1 ){
				_associatedIndexes.insert( ( size_t )idx );

				// update the current position offset to the one of the fast feature
				Eigen::Matrix3f& m = patch->pose().transformation();
				m( 0, 2 ) = _currentFeatures[ idx ].pt.x;
				m( 1, 2 ) = _currentFeatures[ idx ].pt.y;

				// now try to track with klt in multiscale fashion
				if( _klt.trackPatchMultiscale( *patch, _pyramid ) ){
					tracked.push_back( patch );
				} 
			}
		}
	}

	void FASTFeatureTracking::detectCurrentFeatures( const Image& img )
	{
		// create the scale space
		_pyramid.update( img );

		float scale = 1.0f;
		// detect for each scale
		for( size_t i = 0; i < _pyramid.octaves(); i++ ){
			std::vector<Feature2Df> scaleFeatures;	
			VectorFeature2DInserter<float> inserter( scaleFeatures );
			_detector.extract( _pyramid[ i ], inserter );

			size_t start = _currentFeatures.size();

			// insert the features
			_currentFeatures.insert( _currentFeatures.end(), scaleFeatures.begin(), scaleFeatures.end() );
			if( i != 0 ){
				scale /= _pyramid.scaleFactor();
				for( size_t f = start; f < _currentFeatures.size(); f++ ){
					_currentFeatures[ f ].pt *= scale;
				}
			}
		}

	}

	int FASTFeatureTracking::bestFASTFeatureInRadius( const Image& img, const PatchType& patch )
	{
		Vector2f patchPos;
		patch.currentCenter( patchPos );

		IMapScoped<const uint8_t> map( img );
		
		SIMD* simd = SIMD::instance();
		std::set<size_t>::const_iterator assocEnd = _associatedIndexes.end();

		static const float normalizer = 1.0f / ( Math::sqr( PatchSize ) * 255 );

		float best = _fastMinMatchingThreshold;
		int idx = -1;
		for( size_t i = 0; i < _currentFeatures.size(); i++ ){
			// avoid double associations
			if( _associatedIndexes.find( i ) != assocEnd )
				continue;

			const Vector2f& fpos = _currentFeatures[ i ].pt;
			if( ( fpos - patchPos ).lengthSqr() < _fastMatchingWindowSqr ){
				// within radius: check the SAD value
				const uint8_t* p2 = map.ptr() + ( int )fpos.y * map.stride() + ( int )fpos.x;

				float sad = 1.0f - normalizer * simd->SAD( patch.pixels(), p2, Math::sqr( PatchSize ) );
				
				if( sad > best ){
					best = sad;
					idx = i;
				}
			}
		}

		return idx;
	}
}
