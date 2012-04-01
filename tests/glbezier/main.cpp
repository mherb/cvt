#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/GLView.h>

#include <cvt/gl/progs/GLBasicProg.h>

using namespace cvt;

static const char* _frag = "#version 150\n" \
"\n" \
"in vec2 Texcoord;\n" \
"in vec4 Color;\n" \
"out vec4 Output;\n" \
"\n" \
"void main()\n" \
"{\n" \
"	if( Texcoord.x * Texcoord.x - Texcoord.y >= 0.0 )\n" \
"		discard; \n" \
"	else\n" \
"		Output = Color; \n" \
"}\n" \
"\n";

static const char _vert[ ] =
"#version 150\n" \
"\n" \
"uniform mat4 MVP;\n" \
"\n" \
"in vec3 in_Vertex;\n" \
"in vec4 in_Color;\n" \
"out vec4 vtx_Color;\n" \
"\n" \
"void main()\n" \
"{\n" \
"	vtx_Color = in_Color;\n" \
"	gl_Position = MVP * vec4( in_Vertex, 1.0 );\n" \
"}\n" \
"\n";

static const char _geom[] =
"#version 150\n" \
"uniform mat4 MVP;\n" \
"layout(triangles) in;\n" \
"layout(triangle_strip, max_vertices=6) out;\n" \
"in vec4 vtx_Color[];\n" \
"out vec4 Color;\n" \
"out vec2 Texcoord;\n" \
"void main(void)\n" \
"{\n" \
"	gl_Position = gl_in[ 0 ].gl_Position;\n" \
"	Color = vtx_Color[ 0 ];\n" \
"	Texcoord = vec2( 0, 0 );\n" \
"	EmitVertex();\n" \
"	gl_Position = gl_in[ 1 ].gl_Position;\n" \
"	Color = vtx_Color[ 1 ];\n" \
"	Texcoord = vec2( 0.5, 0 );\n" \
"	EmitVertex();\n" \
"	gl_Position = gl_in[ 2 ].gl_Position;\n" \
"	Color = vtx_Color[ 2 ];\n" \
"	Texcoord = vec2( 1 , 1 );\n" \
"	EmitVertex();\n" \
"	EndPrimitive();\n" \
"	\n" \
"	gl_Position = gl_in[ 2 ].gl_Position;\n" \
"	Color = vtx_Color[ 0 ];\n" \
"	Texcoord = vec2( 0, 1 );\n" \
"	EmitVertex();\n" \
"	gl_Position = MVP * vec4( 0, 0, 0, 1 );\n" \
"	Color = vtx_Color[ 1 ];\n" \
"	Texcoord = vec2( 0, 1 );\n" \
"	EmitVertex();\n" \
"	gl_Position = gl_in[ 0 ].gl_Position;\n" \
"	Color = vtx_Color[ 2 ];\n" \
"	Texcoord = vec2( 0, 1 );\n" \
"	EmitVertex();\n" \
"	EndPrimitive();\n" \
"}\n";

class MyGLView : public GLView
{
	public:
		MyGLView()
		{
			for( int i = 0; i < 8; i++ ) {
				float val = ( ( float ) i / 8.0f ) * Math::TWO_PI;
				_tri[ i ].set( sin( val ) * 100 + 200, cos( val ) * 100 + 200 );
			}

			_buffer.alloc( GL_STREAM_DRAW, sizeof( float ) * 2 * 3 * 4 );
			try {
				_glprog.build( _vert, _frag, _geom );
			} catch( GLException& e ) {
				std::cout << e.what() << std::endl;
				std::cout << e.log() << std::endl;
			}
		}

	protected:

		void paintEvent( PaintEvent* ev, GFX* g )
		{
			GLView::paintEvent( ev, g );

			g->color().set( 1.0f, 0.0f, 0.0f, 0.5f );
			for( int i = 0; i < 8; i++ )
				g->fillRoundRect( _tri[ i ].x - 5, _tri[ i ].y - 5,  10, 10, 5.0f );

		}

		void paintGLEvent( PaintEvent* )
		{
			Recti r = rect();
			setViewport( 0, 0, r.width, r.height );

			Matrix4f proj;
			GL::ortho( proj, 0, r.width, 0, r.height, -100, 100, false );

			_basicProg.bind();

			glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
			glClear( GL_COLOR_BUFFER_BIT );

			_glprog.bind();
			_glprog.setArg( "MVP", proj );

			_vao.setColor( Color::GREEN );
			std::vector<float> vec;
			Vector2f min( r.width , r.height ), max( 0, 0 );

			for( int i = 0; i < 8; i++ ) {
				min.x = Math::min( min.x, _tri[ i ].x );
				min.y = Math::min( min.y, _tri[ i ].y );
				max.x = Math::max( max.x, _tri[ i ].x );
				max.y = Math::max( max.y, _tri[ i ].y );
			}
			for( int i = 0; i < 8; i += 2 ) {
				vec.push_back( _tri[ i ].x );
				vec.push_back( _tri[ i ].y );
				vec.push_back( _tri[ i + 1 ].x );
				vec.push_back( _tri[ i + 1 ].y );
				vec.push_back( _tri[ ( i + 2 ) % 8 ].x );
				vec.push_back( _tri[ ( i + 2 ) % 8 ].y );
			}

			#define STENCILEVENODD 1
			//#define STENCILWINDING 1
#ifdef STENCILEVENODD
			glEnable( GL_STENCIL_TEST );
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glStencilMask( 0x01 );
			glStencilOp( GL_KEEP, GL_KEEP, GL_INVERT );
			glStencilFunc (GL_ALWAYS, 0, ~0);
#endif
#ifdef STENCILWINDING
			glEnable( GL_STENCIL_TEST );
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glStencilMask( ~0 );
			glStencilOpSeparate( GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP );
			glStencilOpSeparate( GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP );
			glStencilFunc( GL_ALWAYS, 0, ~0 );
#endif


			_buffer.setData( sizeof( float ) * 2 * 3 * 4, ( float* ) &vec[ 0 ] );
			_vao.setVertexData( _buffer, 2, GL_FLOAT );
			_vao.draw( GL_TRIANGLES, 0, 3 * 4 );
#ifdef STENCILEVENODD
			glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glStencilFunc(GL_EQUAL, 0x01, 0x01);
			glStencilMask( ~0 );
			glStencilOp( GL_ZERO, GL_ZERO, GL_ZERO );
#endif
#ifdef STENCILWINDING
			glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
			glStencilFunc( GL_NOTEQUAL, 0, ~0 );
			glStencilMask( ~0 );
			glStencilOp( GL_ZERO, GL_ZERO, GL_ZERO );
#endif


			_glprog.unbind();
			_basicProg.bind();
			_basicProg.setProjection( proj );
			_basicProg.setColor( Color( 1.0f, 0.0f, 0.0f, 0.25f ) );
			_basicProg.fillRect( min.x, min.y, max.x - min.x, max.y - min.y );
			_basicProg.unbind();

#if defined( STENCILEVENODD ) || defined( STENCILWINDING )
			glDisable( GL_STENCIL_TEST );
#endif
		}


	void mousePressEvent( MousePressEvent* event )
	{
		Vector2f d, p;
		p.set( event->x, event->y );
		for( int i = 0; i < 8; i++ ) {
			d = _tri[ i ] - p;
			if( d.length() < 10.0f ) {
				_selection = i;
				return;
			}
		}
		_selection = -1;
	}

	void mouseMoveEvent( MouseMoveEvent* event )
	{
		if( _selection < 0 )
			return;
		Vector2f p;
		p.set( event->x, event->y );
		_tri[ _selection ] = p;
		update();
	}

	void mouseReleaseEvent( MouseReleaseEvent* e )
	{
		_selection = -1;
	}

	void resizeEvent( ResizeEvent* e )
	{
		update();
	}



	private:
		int			_selection;
		Vector2f	_tri[ 8 ];

		GLVertexArray _vao;
		GLBuffer	  _buffer;
		GLProgram	  _glprog;
		GLBasicProg	_basicProg;
};

int main( int argc, char* argv[] )
{
	Window w( "Main" );

	w.setSize( 640, 480 );
	MyGLView mgl;

	WidgetLayout wl;
	wl.setAnchoredTopBottom( 0, 0 );
	wl.setAnchoredLeftRight( 0, 0 );
	w.addWidget( &mgl, wl );

	w.setVisible( true );
	mgl.setVisible( true );

	Application::run();

	return 0;
}
