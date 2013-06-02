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

			void classify( RDFClassHistogram<N>& hist, const DATA& d );
		private:

			RDFNode<DATA,RDFClassHistogram<N> >* _root;
	};
}


#endif
