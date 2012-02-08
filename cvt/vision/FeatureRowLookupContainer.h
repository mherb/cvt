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
			
			void setNumRows( size_t numRows );
			void clear( int row = -1 );

			void operator()( float x, float y );

			RowIterator		 rowBegin( size_t row )	{ return _featureTable[ row ].begin(); }
			RowIterator		 rowEnd( size_t row )	{ return _featureTable[ row ].end(); }
			ConstRowIterator rowBegin( size_t row ) const { return _featureTable[ row ].begin(); }
			ConstRowIterator rowEnd( size_t row )   const { return _featureTable[ row ].end(); }

		private:
			std::vector< std::vector<Feature2Df> > _featureTable;
	};

	inline FeatureRowLookupContainer::FeatureRowLookupContainer( size_t numRows )
	{
		setNumRows( numRows );
	}


	inline void FeatureRowLookupContainer::setNumRows( size_t numRows )
	{
		if( numRows != _featureTable.size() )
			_featureTable.resize( numRows );
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
		_featureTable[ ( size_t )y ].push_back( Feature2Df( x, y  ) );
	}
}

#endif
