#include <cvt/gl/OpenGL.h>

#include <cstdlib>

namespace cvt {
	unsigned int GL::_glmajor = 0;
	unsigned int GL::_glminor = 0;
	unsigned int GL::_glslmajor = 0;
	unsigned int GL::_glslminor = 0;
	std::vector<std::string*> GL::_extensions;

	void ( *GL::glBindVertexArray )( GLuint array ) = NULL;
	void ( *GL::glDeleteVertexArrays )( GLsizei n, const GLuint *arrays ) = NULL;
	void ( *GL::glGenVertexArrays )( GLsizei n, GLuint *arrays) = NULL;
	GLboolean ( *GL::glIsVertexArray )( GLuint array) = NULL;


	void GL::parseExtensions( const char* str )
	{
		const char* sptr = str;
		const char* eptr;
		std::string* ext;

		do {
			while( *sptr && *sptr == ' ' )
				sptr++;
			eptr = sptr;
			while( *eptr && *eptr != ' ' )
				eptr++;
			if( sptr != eptr ) {
				ext = new std::string( sptr, eptr - sptr );
				_extensions.push_back( ext );
				sptr = eptr;
			}
		} while( *eptr );

	}

	void GL::parseVersion( const char* str, unsigned int* major, unsigned int* minor )
	{
		const char* sptr = str;
		char* eptr;

		if( !str ){
			*major = 0;
			*minor = 0;
			return;
		}

		*major = ( unsigned int ) strtol( sptr, &eptr, 10 );
		if( sptr == eptr )
			return;
		while( *eptr && ( *eptr == '.' || *eptr == ' ' ) )
			eptr++;
		sptr = eptr;
		*minor = ( unsigned int ) strtol( sptr, &eptr, 10 );
	}

	void GL::init()
	{
		static bool init = false;
		const GLubyte* str;
		if( init )
			return;
		init = true;

		/* version check */
		str = glGetString( GL_VERSION );
		parseVersion( ( const char* ) str, &_glmajor, &_glminor );
		str = glGetString( GL_SHADING_LANGUAGE_VERSION );
		parseVersion( ( const char* ) str, &_glslmajor, &_glslminor );

		if( _glmajor < 2  || ( _glmajor == 2 && _glminor == 0 ) ) {
			std::cerr << "GL Version too old, at least GL 2.1 is needed" << std::endl;
			std::exit( EXIT_FAILURE );
		}

		if( _glslmajor == 1 && _glslminor < 20 ) {
			std::cerr << "GLSL Version too old, at least GLSL 1.20 is needed" << std::endl;
			std::exit( EXIT_FAILURE );
		}

		/* extensions check */
		str = glGetString( GL_EXTENSIONS );
		parseExtensions( ( const char* ) str );

		if( existsExtension( "GL_ARB_vertex_array_object" ) ) {
			glGenVertexArrays = ( void (*)( GLsizei, GLuint* ) ) getProcAddress( "glGenVertexArrays" );
			glDeleteVertexArrays = ( void (*)( GLsizei, const GLuint* ) ) getProcAddress( "glDeleteVertexArrays" );
			glBindVertexArray = ( void (*)( GLuint ) ) getProcAddress( "glBindVertexArray" );
			glIsVertexArray = ( GLboolean (*)( GLuint ) ) getProcAddress( "glIsVertexArray" );
		} else if( existsExtension( "GL_APPLE_vertex_array_object" ) ) {
			glGenVertexArrays = ( void (*)( GLsizei, GLuint* ) ) getProcAddress( "glGenVertexArraysAPPLE" );
			glDeleteVertexArrays = ( void (*)( GLsizei, const GLuint* ) ) getProcAddress( "glDeleteVertexArraysAPPLE" );
			glBindVertexArray = ( void (*)( GLuint ) ) getProcAddress( "glBindVertexArrayAPPLE" );
			glIsVertexArray = ( GLboolean (*)( GLuint ) ) getProcAddress( "glIsVertexArrayAPPLE" );
		} else {
			std::cerr << "GL vertex array object extension missing" << std::endl;
			std::exit( EXIT_FAILURE );
		}

		if( !existsExtension( "GL_ARB_texture_non_power_of_two" ) ) {
			std::cerr << "GL texture non power of two extension missing" << std::endl;
			std::exit( EXIT_FAILURE );
		}

//		glEnable( GL_FRAMEBUFFER_SRGB_EXT );
	}

	bool GL::existsExtension( const std::string& extname )
	{
		for( std::vector<std::string*>::const_iterator it = _extensions.begin(); it != _extensions.end(); it++ ) {
			if( !extname.compare( **it ) ) {
				return true;
			}
		}
		return false;
	}

	void (* GL::getProcAddress(const char* str ))()
	{
/*#ifdef APPLE
#else*/
		return glXGetProcAddressARB( ( const GLubyte * ) str );
/*#endif*/
	}

	void GL::info( std::ostream& out )
	{
		out << "GL-Version: " << _glmajor << "." << _glminor << std::endl;
		out << "GLSL-Version: " << _glslmajor << "." << _glslminor << std::endl;
		out << "GL Extensions: " << std::endl;
		for( std::vector<std::string*>::const_iterator it = _extensions.begin(); it != _extensions.end(); it++ )
			out << "\t" << **it << std::endl;
	}

	void GL::ortho2d( Matrix4f& mat, float left, float right, float top, float bottom, float near, float far )
	{
		mat.zero();
		mat[ 0 ][ 0 ] = 2.0f / ( right - left );
		mat[ 0 ][ 3 ] = - ( right + left ) / ( right - left );

		mat[ 1 ][ 1 ] = 2.0f / ( top - bottom );
		mat[ 1 ][ 3 ] = - ( top + bottom ) / ( top - bottom );

		mat[ 2 ][ 2 ] = -2.0f / ( far - near );
		mat[ 2 ][ 3 ] = - ( far + near ) / ( far - near );

		mat[ 3 ][ 3 ] = 1.0f;
	}
}
