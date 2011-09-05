#ifndef CVT_FEATURE2DROC_H
#define CVT_FEATURE2DROC_H

#include <cvt/vision/FeatureMatch.h>
#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/util/String.h>

namespace cvt {
	class Feature2DROC {
		public:
			Feature2DROC( const std::vector<FeatureMatch>& matches, const Matrix3f& gthomography, float threshold = 5.0f );
			//void toTXTFile( const String& path ) const;

		private:
			void findMatchesAndMaxDistance( const std::vector<FeatureMatch>& matches );
			void calcRocPoints( const std::vector<FeatureMatch>& matches );

			Matrix3f			 _gthomo; // Ground-truth homography
			float				 _gtthres; // Max distance for match || H * x - x' || < _gtthres -> match
			float				 _maxdistance; // max distance
			std::vector<bool>	 _match; // bool indicates if index i is an match according to _gthomo
			std::vector<Point2f> _rocpts;
	};


	inline Feature2DROC::Feature2DROC( const std::vector<FeatureMatch>& matches, const Matrix3f& gth, float threshold ) :
		_gthomo( gth ),
		_gtthres( threshold ),
		_maxdistance( 0 )
	{
		findMatchesAndMaxDistance( matches );
		calcRocPoints( matches );
	}

	inline void Feature2DROC::findMatchesAndMaxDistance( const std::vector<FeatureMatch>& matches )
	{
		std::vector<FeatureMatch>::const_iterator it = matches.begin();
		std::vector<FeatureMatch>::const_iterator itEnd = matches.end();

		while( it != itEnd ){
			Vector2f gt = _gthomo * it->feature0->pt;

			float dist = ( it->feature1->pt - gt ).length();
			if( dist < _maxdistance )
				_match.push_back( 1 );
			else
				_match.push_back( 0 );
			if( it->distance > _maxdistance )
				_maxdistance = it->distance;
			it++;
		}
	}

	inline void Feature2DROC::calcRocPoints( const std::vector<FeatureMatch>& matches )
	{
		float threshold;
		for( size_t i = 0; i < 102; i++ ) {
			threshold = ( _maxdistance / 100.0f ) * ( float ) i;

			size_t tp = 0;
			size_t actualCorrect = 0;
			size_t fp = 0;
			size_t actualError = 0;

			for( size_t k = 0; k < matches.size(); k++ ) {
				if( _match[ k ] ) {
					actualCorrect++;
					if( matches[ k ].distance < threshold )
						tp++;
				} else {
					actualError++;
					if( matches[ k ].distance < threshold )
						fp++;
				}
			}
			Vector2f rocpt( ( float ) tp / ( float ) actualCorrect, ( float ) fp / ( float ) actualError );
			_rocpts.push_back( rocpt );
		}
	}
}

#endif
