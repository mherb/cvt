#include <iostream>
#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/BasicTimer.h>
#include <cvt/io/ImageIO.h>
#include <cvt/io/Resources.h>
#include <cvt/util/Time.h>

#include <cvt/gl/GLVertexArray.h>

#include <cvt/gl/shader/150/basiclight_150_vert.h>
#include <cvt/gl/shader/150/basiclight_150_frag.h>

using namespace cvt;

class View3d : public Widget
{
	public:
		View3d();
		~View3d();
	protected:
		virtual void paintEvent( PaintEvent* , GFX* );

		GLVertexArray _vao;
		GLProgram	  _glprog;
		GLBuffer	  vtx, normal;
};

View3d::View3d() : Widget()
{
	try {
	_glprog.build( _basiclight_150_vert_source, _basiclight_150_frag_source );
	} catch( GLException e )
	{
		std::cout << e.what() << std::endl << e.log() << std::endl;
	}

	vtx.alloc( GL_STATIC_DRAW, 24 * sizeof( float ) );
	normal.alloc( GL_STATIC_DRAW, 24 * sizeof( float ) );

	Vector3f* buf = ( Vector3f* ) vtx.map( GL_READ_WRITE );
	/* face1 */
	buf[ 0 ].set( -1.0f, -1.0f, 0.0f );
	buf[ 1 ].set( -1.0f,  1.0f, 0.0f );
	buf[ 2 ].set(  1.0f, -1.0f, 0.0f );
	buf[ 3 ].set(  1.0f,  1.0f, 0.0f );


/*	buf[ 0 ] = -1.0f;
	buf[ 1 ] = -1.0f;
	buf[ 2 ] = 0.0f;

	buf[ 0 + 3 ] = -1.0f;
	buf[ 1 + 3 ] = 1.0f;
	buf[ 2 + 3 ] = 0.0f;

	buf[ 0 + 6 ] = 1.0f;
	buf[ 1 + 6 ] = -1.0f;
	buf[ 2 + 6 ] = 0.0f;

	buf[ 0 + 9 ] = 1.0f;
	buf[ 1 + 9 ] = 1.0f;
	buf[ 2 + 9 ] = 0.0f;*/

	vtx.unmap();


	Vector3f* v = ( Vector3f* ) vtx.map( GL_READ_WRITE );
	Vector3f* norm = ( Vector3f* ) normal.map( GL_READ_WRITE );
	Vector3f a, b;
	for( int i = 0;  i < 4; i++ ) {
		norm[ i ].set( 0.0f, 0.0f, -1.0f );
	}
	vtx.unmap();
	normal.unmap();

	_vao.setVertexData( vtx, 3, GL_FLOAT );
	_vao.setNormalData( normal, 3, GL_FLOAT );

}

View3d::~View3d()
{

}

void View3d::paintEvent( PaintEvent* , GFX* g )
{
	static int R = 0;
	int w, h, x, y;
	position( x , y );
	size( w, h );
	g->color().set( 1.0f, 1.0f, 1.0f, 1.0f );
	g->fillRect( 0, 0, w, h );


	Matrix4f vw;
	vw.identity();
	vw[ 0 ][ 0 ] = w / 640.0f;
	vw[ 0 ][ 2 ] = ( x + ( w - 640.0f ) / 2.0f ) / 320.0f;
	vw[ 1 ][ 1 ] = h / 480.0f;
	vw[ 1 ][ 2 ] = ( -y - ( h - 480.0f ) / 2.0f ) / 240.0f;


	Matrix4f proj;
	GL::perspective( proj, 60.0f, 640.0f / 480.0f, 0.01f, 100.0f );

	Matrix4f trans;
	trans.identity();
	trans[ 2 ][ 3 ] = -3.0f;

	R++;
	if( R > 360 )
		R = 0;
	Matrix4f rot;
	float alpha = ( float ) R;
	rot.identity();
	rot[ 1 ][ 1 ] = Math::cos( Math::deg2Rad( alpha ) );
	rot[ 1 ][ 2 ] = -Math::sin( Math::deg2Rad( alpha ) );
	rot[ 2 ][ 1 ] = Math::sin( Math::deg2Rad( alpha ) );
	rot[ 2 ][ 2 ] = Math::cos( Math::deg2Rad( alpha ) );
	alpha = 30.0f;
	Matrix4f rot2;
	rot2.identity();
	rot2[ 0 ][ 0 ] = Math::cos( Math::deg2Rad( alpha ) );
	rot2[ 0 ][ 2 ] = Math::sin( Math::deg2Rad( alpha ) );
	rot2[ 2 ][ 0 ] = -Math::sin( Math::deg2Rad( alpha ) );
	rot2[ 2 ][ 2 ] = Math::cos( Math::deg2Rad( alpha ) );

	proj = vw * proj * trans * rot * rot2;

	Matrix3f nmat;
	for( int y = 0; y < 3; y++ )
		for( int x = 0; x < 3; x++ )
			nmat[ y ][ x ] = proj[ y ][ x ];

	nmat.inverseSelf();
	nmat.transposeSelf();

	_glprog.bind();
	GLint loc = _glprog.uniformLocation( "MVP" );

	glUniformMatrix4fv( loc, 1, true, ( const GLfloat* ) proj.ptr() );
	loc = _glprog.uniformLocation( "NORMM" );
	glUniformMatrix3fv( loc, 1, true, ( const GLfloat* ) nmat.ptr() );
	loc = _glprog.uniformLocation( "LIGHTPOS" );
	Vector3f lightpos( 0.0f, 0.0f, -4.0f );
	glUniform3fv( loc, 1, lightpos.ptr() );
//	glClear( GL_DEPTH_BUFFER_BIT );
//	glEnable( GL_DEPTH_TEST );
	Color c( 1.0f, 0.0f, 0.0f, 1.0f );
	_vao.setColor( c );

	_vao.draw( GL_TRIANGLE_STRIP, 0, 4 );
//	glDisable( GL_DEPTH_TEST );
	_glprog.unbind();
}


void timeout( BasicTimer* timer )
{
    static cvt::Time t;

    std::cout << "Timeout: " << t.elapsedMilliSeconds() << " ms" << std::endl;
    t.reset();
}

int main(int argc, char* argv[])
{
//    BasicTimer t( 30 );
    cvt::Resources resources;
	Image img;
	ImageIO::loadPNG( img, resources.find( "lena.png" ) );

	cvt::Window w( "Test" );
    w.setSize( 640, 480 );
    w.setVisible( true );
    w.setMinimumSize( 320, 240 );

    Button button( "Quit" );
	Delegate<void ()> dquit( &Application::exit );
	button.clicked.add( &dquit );

    WidgetLayout wl;
    wl.setAnchoredRight( 10, 50 );
    wl.setAnchoredBottom( 10, 20 );
    w.addWidget( &button, wl );

	ImageView view;
	Moveable m( &view );
	m.setSize( 200, 200 );
	w.addWidget( &m );

	view.setImage( img );

	View3d v3d;
	Moveable m2( &v3d );
	m2.setSize( 160, 120 );
	m2.setPosition( 240, 180 );
	w.addWidget( &m2 );

  //  Delegate<void (BasicTimer*)> d( &timeout );
    //t.timeout.add( &d );
    //t.start();

    Application::run();
    return 0;
}
