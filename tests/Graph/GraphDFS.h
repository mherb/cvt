#ifndef CVT_GRAPHDFS_H
#define CVT_GRAPHDFS_H

#include "GraphNode.h"

namespace cvt {
	template<typename TNODE,typename TEDGE>
		class GraphDFSVisitor {
			virtual void initNode( GraphNode* g ) {};
			virtual void discoverNode( GraphNode* g ) {};
			virtual void finishNode( GraphNode* g ) {};
		};
}

#endif
