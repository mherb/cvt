/*
   The MIT License (MIT)

   Copyright (c) 2011 - 2013, Philipp Heise and Sebastian Klose

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include <cvt/gl/scene/GLSShaderDefault.h>

#include <cvt/gl/scene/shader/gls_default_vert.h>
#include <cvt/gl/scene/shader/gls_default_frag.h>

namespace cvt {

	GLSShaderDefault::GLSShaderDefault()
	{
		try {
			if( GL::isGLSLVersionSupported( 1, 50 ) ) {
				build( _gls_default_vert_source, _gls_default_frag_source );
			}
		} catch( GLException e ) {
			std::cout << e.what() << e.log() << std::endl;
		}

		_numlightsloc = uniformLocation( "numlights" );
		_mvploc = uniformLocation( "MVP" );
		_mvloc = uniformLocation( "MVP" );
		_normmloc = uniformLocation( "NORMM" );
	}

    GLSShaderDefault::~GLSShaderDefault()
    {
    }

    void GLSShaderDefault::setNumLights( int n )
    {
		glUniform1i( _numlightsloc, n );
    }

	void GLSShaderDefault::setProjection( const Matrix4f& projection, const Matrix4f& modelview )
	{
		Matrix4f mvp( projection );

        // MVP
		mvp *= modelview;
		glUniformMatrix4fv( _mvploc, 1, true , ( const GLfloat* ) mvp.ptr() );

        // MV
		glUniformMatrix4fv( _mvloc, 1, true , ( const GLfloat* ) modelview.ptr() );

        // NORMM
		Matrix3f normm( modelview );
		normm.transposeSelf();
		normm.inverseSelf();
		glUniformMatrix3fv( _normmloc, 1, true , ( const GLfloat* ) normm.ptr() );
	}

	void GLSShaderDefault::setMaterial( const GLSMaterial& material )
    {
    
    }

}
