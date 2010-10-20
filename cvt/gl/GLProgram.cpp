#include <cvt/gl/GLProgram.h>

namespace cvt {
	GLProgram::GLProgram()
	{
	   program = glCreateProgram();
	}

	GLProgram::~GLProgram()
	{
		glDeleteProgram( program );
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

}
