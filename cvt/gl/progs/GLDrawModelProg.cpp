#include <cvt/gl/progs/GLDrawModelProg.h>

#include <cvt/gl/shader/120/basiclight_120_vert.h>
#include <cvt/gl/shader/120/basiclight_120_frag.h>
#include <cvt/gl/shader/150/basiclight_150_vert.h>
#include <cvt/gl/shader/150/basiclight_150_frag.h>

namespace cvt {
	GLDrawModelProg::GLDrawModelProg()
	{
		try {
			if( GL::isGLSLVersionSupported( 1, 50 ) ) {
				build( _basiclight_150_vert_source, _basiclight_150_frag_source );
			} else {
				build( _basiclight_120_vert_source, _basiclight_120_frag_source );
			}
		} catch( GLException e ) {
			std::cout << e.what() << e.log() << std::endl;
		}

		_mvploc = uniformLocation( "MVP" );
		_normmloc = uniformLocation( "NORMM" );
		_lightposloc = uniformLocation( "LIGHTPOS" );
	}

	GLDrawModelProg::~GLDrawModelProg()
	{

	}

	void GLDrawModelProg::setProjection( const Matrix4f& projection )
	{
		Matrix3f normm( projection );
		glUniformMatrix4fv( _mvploc, 1, true , ( const GLfloat* ) projection.ptr() );
		normm.inverseSelf();
		normm.transposeSelf();
		glUniformMatrix3fv( _normmloc, 1, true , ( const GLfloat* ) normm.ptr() );
	}

	void GLDrawModelProg::setLightPosition( const Vector3f& position )
	{
		glUniform3fv( _lightposloc, 1, ( const GLfloat* ) position.ptr() );
	}

}
