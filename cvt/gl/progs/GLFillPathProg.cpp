#include <cvt/gl/progs/GLFillPathProg.h>
#include <cvt/geom/Polygon.h>
#include <cvt/util/SIMD.h>

#include <cvt/gl/shader/120/basic_120_vert.h>
#include <cvt/gl/shader/120/basic_120_frag.h>
#include <cvt/gl/shader/150/basic_150_vert.h>
#include <cvt/gl/shader/150/basic_150_frag.h>

namespace cvt {
	GLFillPathProg::GLFillPathProg()
	{
		try {
			if( GL::isGLSLVersionSupported( 1, 50 ) ) {
				build( _basic_150_vert_source, _basic_150_frag_source );
			} else {
				build( _basic_120_vert_source, _basic_120_frag_source );
			}
		} catch( GLException e ) {
			std::cout << e.what() << e.log() << std::endl;
		}

		bind();
		_mvploc = uniformLocation( "MVP" );
		unbind();
	}

	GLFillPathProg::~GLFillPathProg()
	{

	}

	void GLFillPathProg::setProjection( const Matrix4f& projection )
	{
		glUniformMatrix4fv( _mvploc, 1, true , ( const GLfloat* ) projection.ptr() );
	}

	void GLFillPathProg::setColor( const Color& color )
	{
		_vao.setColor( color );
	}

	void GLFillPathProg::fillPath( const Pathf& path, GFX::PolygonFillRule frule )
	{
		PolygonSetf polyset( path );
		GLBuffer buf;
		float rectdata[ 8 ];

		if( !polyset.size() )
			return; /* nothing to draw*/

		glEnable( GL_STENCIL_TEST );
		if( frule == GFX::WINDING_EVEN_ODD ) {
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glStencilMask( 0x01 );
			glStencilOp( GL_KEEP, GL_KEEP, GL_INVERT );
			glStencilFunc (GL_ALWAYS, 0, ~0);

			// draw triangle fan
			for( int i = 0, end = polyset.size(); i < end; i++ ) {
				const Polygonf& poly = polyset[ i ];
				size_t size = sizeof( GL_FLOAT ) * 2 * poly.size();
				if( buf.size() < size )
					buf.alloc( GL_STREAM_DRAW, size, &poly[ 0 ] );
				else
					buf.setData( size, &poly[ 0 ] );
			   _vao.setVertexData( buf, 2, GL_FLOAT );
			   _vao.draw( GL_TRIANGLE_FAN, 0, poly.size() );
			}

			glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glStencilFunc(GL_EQUAL, 0x01, 0x01);
			glStencilMask( ~0 );
			glStencilOp( GL_ZERO, GL_ZERO, GL_ZERO );

			// draw quad
			Rectf rect = polyset[ 0 ].bbox();
			for( int i = 1, end = polyset.size(); i < end; i++ ) {
				rect.join( polyset[ i ].bbox() );
			}
			rectdata[ 0 ] = rect.x;
			rectdata[ 1 ] = rect.y + rect.height;
			rectdata[ 2 ] = rect.x;
			rectdata[ 3 ] = rect.y;
			rectdata[ 4 ] = rect.x + rect.width;
			rectdata[ 5 ] = rect.y + rect.height;
			rectdata[ 6 ] = rect.x + rect.width;
			rectdata[ 7 ] = rect.y;
			if( buf.size() < sizeof( GL_FLOAT ) * 8 )
				buf.alloc( GL_STREAM_DRAW, sizeof( GL_FLOAT ) * 2 * 4, rectdata );
			else
				buf.setData( sizeof( GL_FLOAT ) * 8, rectdata );
			_vao.setVertexData( buf, 2, GL_FLOAT );
			_vao.draw( GL_TRIANGLE_STRIP, 0, 4 );
		} else if( frule == GFX::WINDING_NONZERO ) {
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glStencilMask( ~0 );
			glStencilOpSeparate( GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP );
			glStencilOpSeparate( GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP );
			glStencilFunc( GL_ALWAYS, 0, ~0 );

			// draw triangle fan
			for( int i = 0, end = polyset.size(); i < end; i++ ) {
				const Polygonf& poly = polyset[ i ];
				size_t size = sizeof( GL_FLOAT ) * 2 * poly.size();
				if( buf.size() < size )
					buf.alloc( GL_STREAM_DRAW, size, &poly[ 0 ] );
				else
					buf.setData( size, &poly[ 0 ] );
			   _vao.setVertexData( buf, 2, GL_FLOAT );
			   _vao.draw( GL_TRIANGLE_FAN, 0, poly.size() );
			}

			glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
			glStencilFunc( GL_NOTEQUAL, 0, ~0 );
			glStencilMask( ~0 );
			glStencilOp( GL_ZERO, GL_ZERO, GL_ZERO );

			// draw quad
			Rectf rect = polyset[ 0 ].bbox();
			for( int i = 1, end = polyset.size(); i < end; i++ ) {
				rect.join( polyset[ i ].bbox() );
			}
			rectdata[ 0 ] = rect.x;
			rectdata[ 1 ] = rect.y + rect.height;
			rectdata[ 2 ] = rect.x;
			rectdata[ 3 ] = rect.y;
			rectdata[ 4 ] = rect.x + rect.width;
			rectdata[ 5 ] = rect.y + rect.height;
			rectdata[ 6 ] = rect.x + rect.width;
			rectdata[ 7 ] = rect.y;
			if( buf.size() < sizeof( GL_FLOAT ) * 8 )
				buf.alloc( GL_STREAM_DRAW, sizeof( GL_FLOAT ) * 2 * 4, rectdata );
			else
				buf.setData( sizeof( GL_FLOAT ) * 8, rectdata );
			_vao.setVertexData( buf, 2, GL_FLOAT );
			_vao.draw( GL_TRIANGLE_STRIP, 0, 4 );
		}
		glDisable( GL_STENCIL_TEST );
	}

}
