#include <cvt/gl/OpenGL.h>

#include <cstdlib>

namespace cvt {
	unsigned int GL::_glmajor = 0;
	unsigned int GL::_glminor = 0;
	unsigned int GL::_glslmajor = 0;
	unsigned int GL::_glslminor = 0;
	std::vector<std::string*> GL::_extensions;

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
		*major = ( unsigned int ) strtol( sptr, &eptr, 10 );
		if( sptr == eptr )
			return;
		while( *eptr && ( *eptr == '.' || *eptr == ' ' ) )
			eptr++;
		sptr = eptr;
		*minor = ( unsigned int ) strtol( sptr, &eptr, 10 );
	}

	bool GL::init()
	{
		static bool init = false;
		const GLubyte* str;
		if( init )
			return true;
		init = true;

		str = glGetString( GL_VERSION );
		parseVersion( ( const char* ) str, &_glmajor, &_glminor );
		str = glGetString( GL_SHADING_LANGUAGE_VERSION );
		parseVersion( ( const char* ) str, &_glslmajor, &_glslminor );

		if( _glmajor < 2 ) {
			return false;
			if( _glmajor == 2 && _glminor == 0 )
				return false;
		}

		if( _glslmajor == 1 && _glslminor < 20 )
			return false;

		str = glGetString( GL_EXTENSIONS );
		parseExtensions( ( const char* ) str );
		return true;
	}

	void GL::info( std::ostream& out )
	{
		out << "GL-Version: " << _glmajor << "." << _glminor << std::endl;
		out << "GLSL-Version: " << _glslmajor << "." << _glslminor << std::endl;
		out << "GL Extensions: " << std::endl;
		for( std::vector<std::string*>::const_iterator it = _extensions.begin(); it != _extensions.end(); it++ )
			out << "\t" << **it << std::endl;
	}
}
