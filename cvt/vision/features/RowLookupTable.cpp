/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */

#include <cvt/vision/features/RowLookupTable.h>
#include <cvt/vision/features/FeatureSet.h>
#include <cvt/vision/features/FeatureDescriptorExtractor.h>

namespace cvt 
{
	template<> class RowLookupTable<FeatureSet>;
	//template<> class RowLookupTable<FeatureDescriptorExtractor>;

	template <class PointContainer>
	RowLookupTable<PointContainer>::RowLookupTable( const PointContainer& fset ) :
		_features( fset )
	{
		buildIndex();
	}

	template <class PointContainer>
	RowLookupTable<PointContainer>::~RowLookupTable()
	{
	}
			

	template <class PointContainer>
	const typename RowLookupTable<PointContainer>::Row& RowLookupTable<PointContainer>::row( size_t r ) const
	{
		return _rowIndex[ r ];
	}

	template <class PointContainer>
	bool RowLookupTable<PointContainer>::isValidRow( size_t r ) const
	{
		if( r > _maxY )
			return false;
		return row( r ).valid();
	}

	template <class PointContainer>
	void RowLookupTable<PointContainer>::buildIndex()
	{
		if( !_features.size() )
			return;
		_maxY = ( int )_features.back().pt.y;
		_rowIndex.resize( _maxY + 1, Row() );

		int cy = ( int )_features[ 0 ].pt.y;
		Row& row = _rowIndex[ cy ];
		row.start = 0;
		++cy;

		typename PointContainer::CmpY cmp;
		for( ; cy < _maxY; ++cy ){
			int start = std::upper_bound( &_features[ row.start ],
										  &_features[ _features.size() ],
										  _features[ row.start ],
					cmp ) - &_features[ 0 ];
			if( start == row.start ) {
				break;
			}
			row.len = start - row.start;

			cy = ( int ) _features[ start ].pt.y;
			row = _rowIndex[ cy ];
			row.start = start;
		}
		row.len = _features.size() - row.start;
	}
}
