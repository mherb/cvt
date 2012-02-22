/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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

	template<typename TNODE,typename TEDGE>
		class GraphTSVisitor {
			public:
				virtual ~GraphTSVisitor() {};
				virtual void init() {};
				virtual void initNode( GraphNode<TNODE,TEDGE>* ) {};
				virtual void visitNode( GraphNode<TNODE,TEDGE>*, GraphNode<TNODE,TEDGE>* ) {};
		};
}

#endif
