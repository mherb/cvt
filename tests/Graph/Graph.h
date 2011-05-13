#ifndef CVT_GRAPH_H
#define CVT_GRAPH_H

#include "GraphNode.h"
#include <vector>

namespace cvt {

	/* Graph */
	template<typename TNODE, typename TEDGE>
	class Graph {
		public:
			typedef GraphEdge<TNODE,TEDGE> EDGETYPE;
			typedef GraphNode<TNODE,TEDGE> NODETYPE;

			Graph();
			~Graph();

			GraphNode<TNODE,TEDGE>* addNode( TNODE data );
			size_t nodeSize() const;
// FIXME:			size_t edgeSize() const;

		private:
			std::vector< GraphNode<TNODE,TEDGE>* > _nodes;
	};

	template<typename TNODE,typename TEDGE>
	inline Graph<TNODE,TEDGE>::Graph()
	{
	}

	template<typename TNODE,typename TEDGE>
	inline Graph<TNODE,TEDGE>::~Graph()
	{
		for( typename std::vector< GraphNode<TNODE,TEDGE>* >::const_iterator it = _nodes.begin(), end = _nodes.end(); it != end; ++it )
			delete *it;
		_nodes.clear();
	}

	template<typename TNODE,typename TEDGE>
	inline GraphNode<TNODE,TEDGE>* Graph<TNODE,TEDGE>::addNode( TNODE data )
	{
		GraphNode<TNODE,TEDGE>* node = new GraphNode<TNODE,TEDGE>( data );
		_nodes.push_back( node );
		return node;
	}

	template<typename TNODE,typename TEDGE>
	inline size_t Graph<TNODE,TEDGE>::nodeSize() const
	{
		return _nodes.size();
	}
}

#endif
