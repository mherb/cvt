#include "CVTRawLoader.h"
#include <cvt/util/PluginManager.h>
#include <cvt/gfx/Image.h>
#include <iostream>
#include <fstream>

namespace cvt {
	String CVTRawLoader::_extension = ".cvtraw";
	String CVTRawLoader::_name = "CVTRaw";

	void CVTRawLoader::load( Image& img, const String& path )
	{
		std::fstream file( path.c_str(),
						  std::fstream::in |
						  std::fstream::binary );

		// header: width, height, stride, IFormat
		size_t savedStride, width, height, formatId;

		file.read( ( char* )&width, sizeof( size_t ) );
		file.read( ( char* )&height, sizeof( size_t ) );
		file.read( ( char* )&savedStride, sizeof( size_t ) );
		file.read( ( char* )&formatId, sizeof( size_t ) );

		img.reallocate( width, height, IFormat::formatForId( ( IFormatID ) formatId ) );

		char *p, *punmap;
		size_t stride;
		p = punmap = img.map<char>( &stride );

		if( savedStride == stride ){
			file.read( p, height * stride );
		} else {
			size_t offset = savedStride - width;
			while( height-- ){
				file.read( p, width * img.bpp() );
				file.seekg( offset, std::ios::end );
				p += stride;
			}
		}

		img.unmap( punmap );

		file.close();
	}
}

static void _init( cvt::PluginManager* pm )
{
	cvt::ILoader* cvtraw = new cvt::CVTRawLoader();
	pm->registerPlugin( cvtraw );
}

CVT_PLUGIN( _init )
