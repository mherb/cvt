#include <cvt/math/graph/Graph.h>
#include <iostream>

using namespace cvt;

class MyVistor : public GraphVisitor<int,int>
{
	void visitNode( GraphNode<int,int>* node )
	{
		std::cout << node->data() << std::endl;
	}
};

struct nodeint {
	nodeint( int n, int l ) : name( n ), level( l ) {};
	int name;
	int level;
};

class MyTSVistor : public GraphTSVisitor<nodeint,int>
{
	void initNode( GraphNode<nodeint,int>* node )
	{
		node->data().level = 0;
	}

	void visitNode( GraphNode<nodeint,int>* node, GraphNode<nodeint,int>* parent )
	{
		std::cout << "Name: " << node->data().name << std::endl;
		if( parent )
			node->data().level = parent->data().level + 1;
		std::cout << "Level: " << node->data().level << std::endl;
	}
};

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
	MyVistor v;
	g.dfs( v );

	Graph<nodeint,int> g2;
	GraphNode<nodeint,int>* tsnode1 = g2.addNode( nodeint( 1, 0 ) );
	GraphNode<nodeint,int>* tsnode2 = g2.addNode( nodeint( 2, 0 ) );
	GraphNode<nodeint,int>* tsnode3 = g2.addNode( nodeint( 3, 0 ) );
	GraphNode<nodeint,int>* tsnode4 = g2.addNode( nodeint( 4, 0 ) );
	tsnode1->addEdgeTo( tsnode2, 0 );
	tsnode1->addEdgeTo( tsnode3, 0 );
	tsnode3->addEdgeTo( tsnode4, 0 );
	MyTSVistor tv;
	g2.topologicalSort( tv );

}
