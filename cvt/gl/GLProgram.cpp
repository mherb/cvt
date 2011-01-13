#include <cvt/gl/GLProgram.h>
#include <cvt/gfx/Color.h>

namespace cvt {
	GLProgram::GLProgram()
	{
		program = glCreateProgram();
		bindAttribLocation( "in_Vertex", GLSL_VERTEX_IDX );
		bindAttribLocation( "in_Color", GLSL_COLOR_IDX );
		bindAttribLocation( "in_Normal", GLSL_NORMAL_IDX );
		bindAttribLocation( "in_TexCoord", GLSL_TEXCOORD_IDX );
	}

	GLProgram::~GLProgram()
	{
		glDeleteProgram( program );
	}

	void GLProgram::bind()
	{
		glUseProgram( program );
	}

	void GLProgram::unbind()
	{
		glUseProgram( 0 );
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


	void GLProgram::bindAttribLocation( const char *name, unsigned int location )
	{
	    glBindAttribLocation( program, ( GLuint ) location, name );
	}


	unsigned int GLProgram::getAttribLocation( const char* name ) const
	{
	    return glGetAttribLocation( program, name );
	}

	/*void GLProgram::bindFragDataLocation( const char *name, size_t location )
	{
		glBindFragDataLocation( program, ( GLuint ) location, name );
	}*/

	GLint GLProgram::uniformLocation( const char* name ) const
	{
		return glGetUniformLocation( program, name);
	}

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

	void GLProgram::setArg( const char* name, float f1, float f2 )
	{
		GLint loc = glGetUniformLocation( program, name);
		glUniform2f( loc, f1, f2 );
	}
	
	void GLProgram::setArg( const char* name, const Color & c )
	{
		GLint loc = glGetUniformLocation( program, name);
		glUniform4fv( loc, 1, ( GLfloat* )c.data() );
	}
	
	void GLProgram::setArg( const char* name, const cvt::Matrix4f & m )
	{
		GLint loc = glGetUniformLocation( program, name);
		glUniform4fv( loc, 1, ( GLfloat* )m.ptr() );
	}
}
