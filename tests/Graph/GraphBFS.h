#ifndef CVT_GRAPHBFS_H
#define CVT_GRAPHBFS_H

namespace cvt {
	template<typename TNODE,typename TEDGE>
		class GraphBFSVisitor {
			virtual void initNode( GraphNode* g ) {};
			virtual void discoverNode( GraphNode* g ) {};
			virtual void finishNode( GraphNode* g ) {};

		};
}

#endif
