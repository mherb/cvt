
#ifndef CVT_ROWLOOKUPTABLE_H
#define CVT_ROWLOOKUPTABLE_H

#include <vector>
#include <cvt/vision/features/FeatureSet.h>
#include <cvt/vision/features/FeatureDescriptorExtractor.h>

namespace cvt 
{
	class RowLookupTable {
		public:
			struct Row
			{
				Row() : start( -1 ), len( 0 )
				{
				}

				int		start;
				size_t	len;
				bool valid() const { return start != -1 && len > 0; }
			};

			/**
			 *	@param fset		the sorted featureset to operate on
			 */
			RowLookupTable( const FeatureSet& fset );

			/**
			 * @brief RowLookupTable
			 * @param constructor for feature descriptorset
			 */
			RowLookupTable( const FeatureDescriptorExtractor& fdset );
			~RowLookupTable();

			const Row& row( size_t r ) const;
			bool  isValidRow( int r ) const;
			
		private:
			int						_maxY, _minY;
			std::vector<Row>		_rowIndex;
			void buildIndex( const FeatureSet& fset );
			void buildIndex( const FeatureDescriptorExtractor& fset );

	};
}

#endif
