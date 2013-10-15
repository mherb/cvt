/*
   The MIT License (MIT)

   Copyright (c) 2011 - 2013, Philipp Heise and Sebastian Klose

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
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
