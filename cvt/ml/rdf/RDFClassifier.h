#ifndef CVT_RDFCLASSIFIER_H
#define CVT_RDFCLASSIFIER_H

#include <vector>
#include <cvt/ml/rdf/RDFClassificationTree.h>

namespace cvt {
	template<typename DATA,size_t N>
	class RDFClassifier {
		public:
			RDFClassifier();
			~RDFClassifier();

			void    addTree( const RDFClassificationTree<DATA,N>* tree );
			size_t  treeCount() const;

			void    classify( RDFClassHistogram<N>& classhist, const DATA& data );

		private:
			std::vector<RDFClassificationTree<DATA,N>*> _trees;
	};

	template<typename DATA, size_t N>
	inline RDFClassifier<DATA,N>::RDFClassifier()
	{
	}

	template<typename DATA, size_t N>
	inline RDFClassifier<DATA,N>::~RDFClassifier()
	{
	}

	template<typename DATA, size_t N>
	inline void RDFClassifier<DATA,N>::addTree( const RDFClassificationTree<DATA,N>* tree )
	{
		_trees.push_back( tree );
	}

	template<typename DATA, size_t N>
	inline size_t RDFClassifier<DATA,N>::treeCount() const
	{
		return _trees.size();
	}

	template<typename DATA, size_t N>
	inline void RDFClassifier<DATA,N>::classify( RDFClassHistogram<N>& chist, const DATA& data )
	{
	}

}

#endif
