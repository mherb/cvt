/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/gl/progs/GLDrawTexModelProg.h>

//#include <cvt/gl/shader/120/basiclight_120_vert.h>
//#include <cvt/gl/shader/120/basiclight_120_frag.h>
#include <cvt/gl/shader/150/basiclighttex_150_vert.h>
#include <cvt/gl/shader/150/basiclighttex_150_frag.h>

namespace cvt {
	GLDrawTexModelProg::GLDrawTexModelProg()
	{
		try {
			if( GL::isGLSLVersionSupported( 1, 50 ) ) {
				build( _basiclighttex_150_vert_source, _basiclighttex_150_frag_source );
			} else {
//				build( _basiclight_120_vert_source, _basiclight_120_frag_source );
			}
		} catch( GLException e ) {
			std::cout << e.what() << e.log() << std::endl;
		}

		bind();
		_mvploc = uniformLocation( "MVP" );
		_normmloc = uniformLocation( "NORMM" );
		_lightposloc = uniformLocation( "LIGHTPOS" );
		_texloc = uniformLocation( "Tex" );
		glUniform1i( _texloc, 0 );
		unbind();
	}

	GLDrawTexModelProg::~GLDrawTexModelProg()
	{

	}

	void GLDrawTexModelProg::setProjection( const Matrix4f& projection, const Matrix4f& modelview )
	{
		Matrix4f mvp( projection );
		mvp *= modelview;
		glUniformMatrix4fv( _mvploc, 1, true , ( const GLfloat* ) mvp.ptr() );
		Matrix3f normm( modelview );
		normm.transposeSelf();
		normm.inverseSelf();
//		std::cout << normm << std::endl;
		glUniformMatrix3fv( _normmloc, 1, true , ( const GLfloat* ) normm.ptr() );
	}

	void GLDrawTexModelProg::setLightPosition( const Vector3f& position )
	{
		glUniform3fv( _lightposloc, 1, ( const GLfloat* ) position.ptr() );
	}

}
