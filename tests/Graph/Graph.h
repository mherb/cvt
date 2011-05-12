#ifndef CVT_GRAPH_H
#define CVT_GRAPH_H

#include <vector>

namespace cvt {
	template<typename,typename> class GraphNode;

	/* Edge */
	template<typename TNODE, typename TEDGE>
	class GraphEdge {
		public:
			typedef GraphNode<TNODE,TEDGE> NODETYPE;

			GraphEdge(  NODETYPE* src, NODETYPE* dst, TEDGE data );
			~GraphEdge();

			TEDGE& data();
			const TEDGE& data() const;
			NODETYPE* source() const;
			NODETYPE* dest() const;

		public:
			TEDGE	  _data;
			NODETYPE* _src;
			NODETYPE* _dst;
	};

	/* Node */
	template<typename TNODE, typename TEDGE>
	class GraphNode {
		public:
			typedef GraphEdge<TNODE,TEDGE> EDGETYPE;
			typedef GraphNode<TNODE,TEDGE> NODETYPE;

			GraphNode( TNODE data );
			~GraphNode();

			TNODE& data();
			const TNODE& data() const;

			size_t inSize();
			size_t outSize();
			size_t size();
			EDGETYPE* addEdgeTo( NODETYPE* node );
			EDGETYPE* addEdgeFrom( NODETYPE* node );

		public:
			TNODE _data;
			std::vector< EDGETYPE* > _inEdges;
			std::vector< EDGETYPE* > _outEdges;
	};

	/* Graph */
	template<typename TNODE, typename TEDGE>
	class Graph {
		public:
			typedef GraphEdge<TNODE,TEDGE> EDGETYPE;
			typedef GraphNode<TNODE,TEDGE> NODETYPE;

			Graph();
			~Graph();

			NODETYPE* addNode( TNODE data );
			size_t nodeSize() const;
			size_t edgeSize() const;

		private:
			std::vector< NODETYPE* > _nodes;
	};
}

#endif
