#ifndef CVT_GRAPHVISITOR_H
#define CVT_GRAPHVISITOR_H

#include <cvt/math/graph/GraphNode.h>

namespace cvt {
	template<typename TNODE,typename TEDGE>
		class GraphVisitor {
			public:
				virtual ~GraphVisitor() {};
				virtual void init() {};
				virtual void initNode( GraphNode<TNODE,TEDGE>* ) {};
				virtual void visitNode( GraphNode<TNODE,TEDGE>* ) {};
		};
}

#endif
