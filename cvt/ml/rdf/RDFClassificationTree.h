#ifndef CVT_RDFCLASSIFICATIONTREE_H
#define CVT_RDFCLASSIFICATIONTREE_H

#include <cvt/ml/rdf/RDFNode.h>
#include <cvt/ml/rdf/RDFClassHistogram.h>

namespace cvt {
	template<typename DATA, size_t N>
	class RDFClassificationTree
	{
		public:
			RDFClassificationTree( RDFNode<DATA,RDFClassHistogram<N> >* root );
			~RDFClassificationTree();

			const RDFClassHistogram<N>& classify( const DATA& d );
		private:
			RDFClassificationTree( const RDFClassificationTree<DATA,N>& );

			RDFNode<DATA,RDFClassHistogram<N> >* _root;
	};


	template<typename DATA, size_t N>
	inline RDFClassificationTree<DATA,N>::RDFClassificationTree( RDFNode<DATA,RDFClassHistogram<N> >* root ) : _root( root )
	{
	}

	template<typename DATA, size_t N>
	inline RDFClassificationTree<DATA,N>::~RDFClassificationTree( )
	{
	}


	template<typename DATA, size_t N>
	inline const RDFClassHistogram<N>& RDFClassificationTree<DATA,N>::classify( const DATA& d )
	{
		RDFNode<DATA,RDFClassHistogram<N> >* node = _root;
		while( !node->isLeaf() ) {
			if( node->test( d ) )
				node = node->right();
			else
				node = node->left();
		}
		return *node->data();
	}

}


#endif
