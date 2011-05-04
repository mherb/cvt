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


		if( !polyset.size() )
			return; /* nothing to draw*/

		glEnable( GL_STENCIL_TEST );
		if( frule == GFX::WINDING_EVEN_ODD ) {
			glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glStencilMask (0x01);
			glStencilOp (GL_KEEP, GL_KEEP, GL_INVERT);
			glStencilFunc (GL_ALWAYS, 0, ~0);

			// draw triangle fan

			glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glStencilFunc (GL_LEQUAL, 0x01, 0x01);
			glStencilOp (GL_ZERO, GL_ZERO, GL_ZERO);
			// draw quad

		} else if( frule == GFX::WINDING_NONZERO ) {
			glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
			glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);
			glStencilFunc(GL_ALWAYS, 0, ~0);

			// draw triangle fan
			glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

			glStencilFunc (GL_NOTEQUAL, 0x0, ~0x0);
			glStencilOp (GL_ZERO, GL_ZERO, GL_ZERO);

			// draw quad
		}
		glDisable( GL_STENCIL_TEST );
	}


}
