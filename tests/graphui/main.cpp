#include <cvt/gui/Window.h>
#include <cvt/gui/Widget.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/Application.h>
#include <cvt/math/graph/Graph.h>
#include <cvt/math/graph/GraphVisitor.h>
#include <cvt/math/Vector.h>
#include <vector>


using namespace cvt;

class NodeUI : public Widget
{
	public:
		NodeUI() : name( 0 ), level( 0 ), _move( false )
		{
			setSize( 50, 50 );
		}
		~NodeUI() {};

		void paintEvent( PaintEvent& , GFX& g )
		{
			g.color().set( 0.0f, 0.8f, 0.0f, 0.5f );
			Recti r;
			size( r.width, r.height );
			r.setPosition( 0, 0 );
			g.fillRoundRect( r, r.width / 2 );

			g.color().set( 0.0f, 0.0f, 0.0f, 1.0f );
			String s;
			s.sprintf("%d - %d", name, level );
			g.drawText( r, ALIGN_CENTER | ALIGN_VCENTER, s.c_str() );
		}

		void mousePressEvent( MousePressEvent& e )
		{
			if( e.button() == 1 ) {
				_move = true;
				e.position( _lx, _ly );
				mapGlobal( _lx, _ly );
			}
		}

		void mouseMoveEvent( MouseMoveEvent& e )
		{
			if( _move ) {
				int dx, dy;
				int cx, cy;
				e.position( cx, cy );
				mapGlobal( cx, cy );
				dx = cx - _lx;
				dy = cy - _ly;
				_lx = cx;
				_ly = cy;
				position( cx, cy );
				setPosition( cx + dx, cy + dy );
				update();
			}
		}

		void mouseReleaseEvent( MouseReleaseEvent& e )
		{
			if( _move )
				_move = false;
		}

		GraphNode<NodeUI*,int>* node()
		{
			return _node;
		}

		void setNode( GraphNode<NodeUI*,int>* node )
		{
			_node = node;
		}

		int name;
		int level;
	private:
		bool _move;
		int _lx, _ly;
		GraphNode<NodeUI*,int>* _node;
};

class NodeUIVisitor : public GraphVisitor<NodeUI*,int>
{
	public:
		NodeUIVisitor() : _n( 0 ) {}
		void init() { _n = 0; }

		void visitNode( GraphNode<NodeUI*,int>* n ) {
			NodeUI* ui = n->data();
			ui->name = _n++;
		}

	private:
		int _n;
};

class NodeUITSVisitor : public GraphTSVisitor<NodeUI*,int>
{
	public:
		NodeUITSVisitor() : _n( 0 ) {}
		void init() { _n = 0; }

		void initNode( GraphNode<NodeUI*,int>* n ) {
			NodeUI* ui = n->data();
			ui->level = 0;
		}

		void visitNode( GraphNode<NodeUI*,int>* n, GraphNode<NodeUI*,int>* parent ) {
			NodeUI* ui = n->data();
			ui->name = _n++;
			if( parent )
				ui->level = parent->data()->level + 1;
			else
				ui->level = 0;
		}

	private:
		int _n;
};

class GraphUI : public Window
{
	public:
		GraphUI( const String& name ) : Window( name ), _dfs( "DFS" ), _bfs( "BFS" ), _ts( "TopSort" ), _addnode("Add node" ), _csrc( NULL ), _cdst( NULL )
	    {
			WidgetLayout wl;

			wl.setAnchoredRight( 10, 150 );
			wl.setAnchoredTop( 10, 20 );
			addWidget( &_dfs, wl );
			Delegate<void ()> ddfs( this, &GraphUI::dfs );
			_dfs.clicked.add( ddfs );

			wl.setAnchoredTop( 35, 20 );
			addWidget( &_bfs, wl );
			Delegate<void ()> bdfs( this, &GraphUI::bfs );
			_bfs.clicked.add( bdfs );

			wl.setAnchoredTop( 60, 20 );
			addWidget( &_ts, wl );
			Delegate<void ()> ts( this, &GraphUI::ts );
			_ts.clicked.add( ts );

			wl.setAnchoredTop( 85, 20 );
			addWidget( &_addnode, wl );
			Delegate<void ()> d( this, &GraphUI::addNode );
			_addnode.clicked.add( d );

		};

		void paintEdge( NodeUI* n, int cx, int cy, GFX& g )
		{
			int px, py;
			n->position( px, py  );

			Vector2f norm( px + 25.0f - cx, py + 25.0f - cy );
			if( norm.length() < 25.0f )
				return;
			norm.normalize();
			g.drawLine( px + 25 - 25.0f * norm.x, py + 25 - 25.0f * norm.y, cx, cy );
		}

		void paintEdge( NodeUI* n1, NodeUI* n2, GFX& g )
		{
			int px, py, cx, cy;
			n1->position( px, py );
			n2->position( cx, cy );

			Vector2f norm( px - cx, py - cy );
			norm.normalize();
			g.drawLine( px + 25 - 25.0f * norm.x, py + 25 - 25.0f * norm.y, cx + 25 + 25.0f * norm.x, cy + 25 + 25.0f * norm.y );
			Vector2f normr( -norm.y, norm.x );
			g.drawLine( cx + 25 + 25.0f * norm.x, cy + 25 + 25.0f * norm.y, cx + 25 + 30.0f * norm.x + 5.0f * normr.x , cy + 25 + 30.0f * norm.y + 5.0f * normr.y  );
			g.drawLine( cx + 25 + 25.0f * norm.x, cy + 25 + 25.0f * norm.y, cx + 25 + 30.0f * norm.x - 5.0f * normr.x , cy + 25 + 30.0f * norm.y - 5.0f * normr.y  );
		}

		void paintEvent( PaintEvent& pe, GFX& g )
		{
			g.setLineWidth( 1.0f );
			Recti r;
			r.setPosition( 0, 0 );
			size( r.width, r.height );
			g.color().set( 0.6f, 0.6f, 0.6f, 1.0f );
			g.fillRect( r );

			if( _csrc ) {
				g.color().set( 0.0f, 0.0f, 0.0f, 1.0f );
				g.setLineWidth( 1.0f );
				if( _cdst && _cdst != _csrc )
					paintEdge( _csrc, _cdst, g );
				else
					paintEdge( _csrc, _cx, _cy, g );
			}

			g.color().set( 0.0f, 0.0f, 0.0f, 1.0f );
			for( size_t i = 0, iend = _graph.nodeSize(); i < iend; i++ ) {
				GraphNode<NodeUI*,int>* node = _graph.node( i );
				for( size_t k = 0, kend = node->outSize(); k < kend; k++ ) {
					paintEdge( node->data(), node->outEdge( k )->dest()->data(), g );
				}
			}

			paintChildren( g, r );
		}

		void mousePressEvent( MousePressEvent& e )
		{
			NodeUI* node;
			if( ( node = dynamic_cast<NodeUI*>( childAt( e.x, e.y ) ) ) && e.button() == 3 ) {
				_csrc = node;
				_cx = e.x;
				_cy = e.y;
				update();
			} else
				Window::mousePressEvent( e );
		}

		void mouseMoveEvent( MouseMoveEvent& e )
		{
			if( _csrc ) {
				_cdst = dynamic_cast<NodeUI*>( childAt( e.x, e.y ) );
				_cx = e.x;
				_cy = e.y;
				update();
			} else
				Window::mouseMoveEvent( e );
		}

		void mouseReleaseEvent( MouseReleaseEvent&  e )
		{
			if( _csrc ) {
				NodeUI* node;
				if( ( _cdst = dynamic_cast<NodeUI*>( childAt( e.x, e.y ) ) ) )
					_csrc->node()->addEdgeTo( _cdst->node(), 0 );
				_csrc = NULL;
				_cdst = NULL;
				update();
			} else {
				Window::mouseReleaseEvent(e );
			}
		}

		void addNode()
		{
			NodeUI* n = new NodeUI();
			_nodes.push_back( n );
			addWidget( n );
			n->setNode( _graph.addNode( n ) );
			update();
		}

		void dfs()
		{
			NodeUIVisitor v;
			_graph.dfs( v );
			update();
		}

		void bfs()
		{
			NodeUIVisitor v;
			_graph.bfs( v );
			update();
		}

		void ts()
		{
			NodeUITSVisitor v;
			_graph.topologicalSort( v );
			update();
		}

	private:
		Button _dfs;
		Button _bfs;
		Button _ts;
		Button _addnode;
		std::vector<NodeUI*> _nodes;
		Graph<NodeUI*,int> _graph;
		NodeUI* _csrc;
		NodeUI* _cdst;
		int		_cx, _cy;
};



int main()
{
	GraphUI ui( "GraphUI" );
	ui.setSize( 640, 480 );
	ui.show();

	Application::run();
}
