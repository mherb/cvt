#ifndef CVT_GRAPH_H
#define CVT_GRAPH_H

#include <cvt/math/graph/GraphNode.h>
#include <cvt/math/graph/GraphVisitor.h>
#include <cvt/util/Stack.h>
#include <vector>
#include <queue>

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

			void dfs( GraphVisitor<TNODE,TEDGE>& visitor );
			void bfs( GraphVisitor<TNODE,TEDGE>& visitor );

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


	template<typename TNODE,typename TEDGE>
	inline void Graph<TNODE,TEDGE>::dfs( GraphVisitor<TNODE,TEDGE>& visitor )
	{
		Stack<GraphNode<TNODE,TEDGE>*> stack;
		GraphNode<TNODE,TEDGE>* node;
		GraphNode<TNODE,TEDGE>* ntmp;
		size_t n;

		visitor.init();
		node = _nodes[ 0 ];
		n = _nodes.size();
		while( n-- ) {
			node->_visited = false;
			visitor.initNode( node );
			if( !node->inSize() )
				stack.push( node );
		}
		while( !stack.isEmpty() ) {
			node = stack.pop();
			for( size_t i = 0, end = node->outSize(); i < end; i++ ) {
				ntmp = node->outEdge( i )->dest();
				if( !ntmp->_visited )
					stack.push( ntmp );
			}
			if( !node->_visited ) {
				node->_visited = true;
				visitor.visitNode( node );
			}
		}
	}

	template<typename TNODE,typename TEDGE>
	inline void Graph<TNODE,TEDGE>::bfs( GraphVisitor<TNODE,TEDGE>& visitor )
	{
		std::queue<GraphNode<TNODE,TEDGE>*> queue;
		GraphNode<TNODE,TEDGE>* node;
		GraphNode<TNODE,TEDGE>* ntmp;
		size_t n;

		visitor.init();
		node = _nodes[ 0 ];
		n = _nodes.size();
		while( n-- ) {
			node->_visited = false;
			visitor.initNode( node );
			if( !node->inSize() )
				queue.push( node );
		}
		while( !queue.empty() ) {
			node = queue.front();
			queue.pop();
			for( size_t i = 0, end = node->outSize(); i < end; i++ ) {
				ntmp = node->outEdge( i )->dest();
				if( !ntmp->_visited )
					queue.push( ntmp );
			}
			if( !node->_visited ) {
				node->_visited = true;
				visitor.visitNode( node );
			}
		}

	}
}

#endif
