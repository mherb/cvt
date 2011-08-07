#include <cvt/gl/GLTexFont.h>
#include <cvt/util/Exception.h>
#include <stdio.h>

namespace cvt {
	GLTexFont::GLTexFont()
	{
	}

	void GLTexFont::load( const char* path )
	{
		FILE* f;
		f = fopen( path, "r" );
		if( !f )
			throw CVTException( "Unable to open GL-font!" );

		if( fread( &_width, sizeof( int32_t ), 1, f ) != 1 )
			throw CVTException( "Unable to read GL-font!" );
		if( fread( &_height, sizeof( int32_t ), 1, f ) != 1 )
			throw CVTException( "Unable to read GL-font!" );
		if( fread( &_fontsize, sizeof( int32_t ), 1, f ) != 1 )
			throw CVTException( "Unable to read GL-font!" );
		if( fread( &_ascent, sizeof( int32_t ), 1, f ) != 1 )
			throw CVTException( "Unable to read GL-font!" );
		if( fread( &_descent, sizeof( int32_t ), 1, f ) != 1 )
			throw CVTException( "Unable to read GL-font!" );
		if( fread( &_spritesize, sizeof( int32_t ), 1, f ) != 1 )
			throw CVTException( "Unable to read GL-font!" );
		if( fread( &_offx, sizeof( int32_t ), 1, f ) != 1 )
			throw CVTException( "Unable to read GL-font!" );
		if( fread( &_offy, sizeof( int32_t ), 1, f ) != 1 )
			throw CVTException( "Unable to read GL-font!" );

		for( int i = 0; i <= 0xff; i++ ) {
			if( fread( _metric + i, sizeof( int32_t ), 5, f ) != 5 )
				throw CVTException( "Unable to read GL-font!" );
		}

		uint8_t* image = new uint8_t[ _width * _height ];
		if( fread( image, sizeof( uint8_t ), _width * _height, f ) != ( size_t ) _width * _height ) {
			delete[] image;
			throw CVTException( "Unable to read GL-font!" );
		}
		_tex.alloc( GL_RED, _width, _height, GL_RED, GL_UNSIGNED_BYTE, image );
		delete[] image;

		fclose( f );
	}
}
