#include "Graph.h"

using namespace cvt;

int main()
{
	Graph<int,int> g;
	GraphNode<int,int>* node1 = g.addNode( 1 );
	GraphNode<int,int>* node2 = g.addNode( 2 );
	GraphNode<int,int>* node3 = g.addNode( 3 );
	GraphNode<int,int>* node4 = g.addNode( 4 );
	node1->addEdgeTo( node2, 0 );
	node1->addEdgeTo( node3, 0 );
	node3->addEdgeTo( node4, 0 );
}
