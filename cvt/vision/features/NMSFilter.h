#ifndef CVT_NMSFILTER_H
#define CVT_NMSFILTER_H

#include <cvt/vision/features/Feature.h>
#include <cvt/vision/features/FeatureSet.h>

namespace cvt {

	class NMSFilter
	{
		public:
			NMSFilter( std::vector<Feature>& features );
			~NMSFilter();

			void filter( int radius );
		private:
			struct ScanlineIndex {
				int offset;
				int length;

				void setInvalid() { offset = -1; }
				bool isValid() { return offset >= 0; }
			};

			void buildScanlineIndex();
			bool isValidY( int y );

			std::vector<Feature>& _features;
			ScanlineIndex*		  _yindex;
			int					  _maxY;

	};

	inline NMSFilter::NMSFilter( std::vector<Feature>& features ) : _features( features ), _yindex( 0 ), _maxY( -1 )
	{
		buildScanlineIndex();
	}

	inline NMSFilter::~NMSFilter()
	{
		delete[] _yindex;
	}

	inline bool NMSFilter::isValidY( int y )
	{
		if( y < 0 || y > _maxY ) return false;
		return _yindex[ y ].isValid();
	}

	inline void NMSFilter::buildScanlineIndex()
	{
		if( !_features.size() )
			return;

		FeatureSet::CmpYi cmp;
		_maxY = ( int ) _features.back().pt.y;

		_yindex = new ScanlineIndex[ _maxY + 1 ];

		int cury = ( int ) _features[ 0 ].pt.y;
		int y = 0;
		for(; y < cury; y++ ) {
			_yindex[ y ].setInvalid();
		}
		_yindex[ y++ ].offset = 0;

		for( ; y <= _maxY; y++ ) {
			int offset = std::upper_bound( _features.begin() + _yindex[ y - 1 ].offset, _features.end(), _features[ _yindex[ y - 1 ].offset ], cmp ) - _features.begin();
			if( offset == _yindex[ y - 1 ].offset ) {
				break;
			}
			_yindex[ y - 1 ].length = offset - _yindex[ y - 1 ].offset;
			cury = ( int ) _features[ offset ].pt.y;
			for(; y < cury; y++ )
				_yindex[ y ].setInvalid();
			_yindex[ y ].offset = offset;
		}
		_yindex[ y - 1 ].length = _features.size() - _yindex[ y - 1 ].offset;
		for(; y <= _maxY; y++ )
			_yindex[ y ].setInvalid();

	}

	void NMSFilter::filter( int radius )
	{
		std::vector<Feature> filtered;
		FeatureSet::CmpXi cmp;

		int iend = _features.size();
		for( int i = 0; i < iend; i++ ) {
			int lowerx = _features[ i ].pt.x - ( float ) radius;
			//int curx = ( ssize_t ) _features[ i ].pt.x;
			int cury = ( ssize_t ) _features[ i ].pt.y;

			for( int r = -radius; r <= radius; r++ ) {
				if( isValidY( cury + r ) ) {
					int begin = _yindex[ cury + r ].offset;
					int end =  begin + _yindex[ cury + r ].length;
					Feature f( lowerx, 0 );
					int x = std::lower_bound( _features.begin() + begin, _features.begin() + end, f , cmp ) - _features.begin();
					while( x < iend &&
						   cury + r == ( int ) _features[ x ].pt.y &&
						   Math::abs( ( int ) _features[ i ].pt.x - ( int ) _features[ x ].pt.x ) <= radius )
					{
						if( _features[ x ].score > _features[ i ].score )
							goto suppressed;
						x++;
					}
				}
			}
			filtered.push_back( _features[ i ] );
			suppressed:
			;
		}

		_features = filtered;
	}
}

#endif
