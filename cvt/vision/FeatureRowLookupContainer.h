/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_FEATURE_ROW_LOOKUP_CONTAINER_H
#define CVT_FEATURE_ROW_LOOKUP_CONTAINER_H

namespace cvt
{
	class FeatureRowLookupContainer
	{
		public:
			typedef std::vector<Feature2Df>::iterator		RowIterator;
			typedef std::vector<Feature2Df>::const_iterator	ConstRowIterator;

			FeatureRowLookupContainer( size_t numRows = 0 );
			
			void	setNumRows( size_t numRows );
			size_t	numRows() const;
			void	clear( int row = -1 );
			
			void	setOctave( size_t o );

			void operator()( float x, float y );

			RowIterator		 rowBegin( size_t row )	{ return _featureTable[ row ].begin(); }
			RowIterator		 rowEnd( size_t row )	{ return _featureTable[ row ].end(); }
			ConstRowIterator rowBegin( size_t row ) const { return _featureTable[ row ].begin(); }
			ConstRowIterator rowEnd( size_t row )   const { return _featureTable[ row ].end(); }

		private:
			std::vector< std::vector<Feature2Df> > _featureTable;
			size_t								   _octave;
	};

	inline FeatureRowLookupContainer::FeatureRowLookupContainer( size_t numRows ) :
		_octave( 0 )
	{
		setNumRows( numRows );
	}


	inline void FeatureRowLookupContainer::setNumRows( size_t numRows )
	{
		if( numRows != _featureTable.size() )
			_featureTable.resize( numRows );
	}

	inline size_t FeatureRowLookupContainer::numRows() const
	{
		return _featureTable.size();
	}

	inline void FeatureRowLookupContainer::setOctave( size_t o )
	{
		_octave = o;
	}

	inline void FeatureRowLookupContainer::clear( int row )
	{
		if( row < 0 ){
			for( size_t i = 0; i < _featureTable.size(); i++ ){
				_featureTable[ i ].clear();
			}
		} else {
			_featureTable[ row ].clear();
		}
	}


	inline void FeatureRowLookupContainer::operator()( float x, float y )
	{
		_featureTable[ ( size_t )y ].push_back( Feature2Df( x, y, 0.0f, _octave ) );
	}
}

#endif
