/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */

#ifndef CVT_ROWLOOKUPTABLE_H
#define CVT_ROWLOOKUPTABLE_H

#include <vector>

namespace cvt 
{
	template <class PointContainer>
	class RowLookupTable {
		public:
			struct Row
			{
				Row() : start( -1 ), len( 0 )
				{
				}

				int		start;
				size_t	len;
				bool valid() const { return start != -1; }
			};

			/**
			 *	@param fset		the sorted featureset to operate on
			 */
			RowLookupTable( const PointContainer& fset );
			~RowLookupTable();

			const Row& row( size_t r ) const;
			bool  isValidRow( size_t r ) const;
			
		private:
			const PointContainer&	_features;
			int						_maxY;
			std::vector<Row>		_rowIndex;

			void buildIndex();

	};
}

#endif
