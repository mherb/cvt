#include <cvt/gl/GLProgram.h>
#include <cvt/gfx/Color.h>
#include <cvt/gfx/IFilterVector.h>
#include <cvt/gfx/IFilterScalar.h>

namespace cvt {
	GLProgram::GLProgram()
	{
	   program = glCreateProgram();
	}

	GLProgram::~GLProgram()
	{
		glDeleteProgram( program );
	}

	void GLProgram::bind()
	{
		glUseProgram( program );
	}

	void GLProgram::build( const char* vertsrc, const char* fragsrc )
	{
		GLuint vs = 0, fs = 0;
		GLint status = GL_TRUE;
		GLint loglen;
		std::string logbuf;

		if( !vertsrc || !fragsrc )
			throw GLException( "Verter/Fragment shader source missing!\n" );

		vs = glCreateShader( GL_VERTEX_SHADER );
		if( !vs )
			throw GLException("Vertex-Shader creation failed!\n");

		glShaderSource( vs, 1, &vertsrc, NULL );
		glCompileShader( vs );
		glGetShaderiv( vs, GL_COMPILE_STATUS, &status );
		if( status == GL_FALSE ) {
			glGetShaderiv( vs, GL_INFO_LOG_LENGTH, &loglen );
			logbuf.resize( loglen );
			glGetShaderInfoLog( vs, loglen, &loglen, &logbuf[ 0 ]  );
			glDeleteShader(vs);
			throw GLException("Vertex-Shader compilation error\n", logbuf );
		}

		fs = glCreateShader( GL_FRAGMENT_SHADER );
		if( !fs ) {
			glDeleteShader( vs );
			throw GLException("Fragment-Shader creation failed!\n");
		}

		glShaderSource( fs, 1, &fragsrc, NULL );
		glCompileShader( fs );
		glGetShaderiv( fs, GL_COMPILE_STATUS, &status );
		if( status == GL_FALSE ) {
			glGetShaderiv( fs, GL_INFO_LOG_LENGTH, &loglen );
			logbuf.resize( loglen );
			glGetShaderInfoLog( fs, loglen, &loglen, &logbuf[ 0 ] );
			glDeleteShader(fs);
			glDeleteShader(vs);
			throw GLException("Fragment-Shader compilation error\n", logbuf );
		}

		glAttachShader( program, fs );
		glAttachShader( program, vs );

		/* shaders are now referenced by programm, delete them*/
		glDeleteShader( vs );
		glDeleteShader( fs );

		glLinkProgram( program );
		glGetProgramiv( program, GL_LINK_STATUS, &status );
		if(status == GL_FALSE) {
			glGetProgramiv( program, GL_INFO_LOG_LENGTH, &loglen );
			logbuf.resize( loglen );
			glGetProgramInfoLog( program, loglen, &loglen, &logbuf[ 0 ] );
			throw GLException("Shader link error\n", logbuf );
		}
	}


	void GLProgram::bindAttribLocation( const char *name, size_t location )
	{
		glBindAttribLocation( program, ( GLuint ) location, name );
	}

	/*void GLProgram::bindFragDataLocation( const char *name, size_t location )
	{
		glBindFragDataLocation( program, ( GLuint ) location, name );
	}*/

	void GLProgram::setArg( const char* name, int i )
	{
		GLint loc = glGetUniformLocation( program, name);
		glUniform1i( loc, i );
	}

	void GLProgram::setArg( const char* name, float f )
	{
		GLint loc = glGetUniformLocation( program, name);
		glUniform1f( loc, f );
	}

	void GLProgram::setArg( const char* name, IFilterParameter* p )
	{
		switch( p->getIFilterParameterType() )
		{
			case IFILTERPARAMETER_COLOR:
				{
					GLint loc = glGetUniformLocation( program, name);
					glUniform4fv( loc, 1, ( GLfloat* ) ( ( Color* ) p )->data() );
				}
				break;
			case IFILTERPARAMETER_SCALAR:
				{
					GLint loc = glGetUniformLocation( program, name);
					glUniform1f( loc, ( ( IFilterScalar* ) p )->get() );
				}
			case IFILTERPARAMETER_VECTOR16:
				{
					GLint loc = glGetUniformLocation( program, name);
					glUniformMatrix4fv( loc, 1, false , ( GLfloat* ) ( ( IFilterVector16* ) p )->vec );
				}
			default:
				break;
		}
	}

}
