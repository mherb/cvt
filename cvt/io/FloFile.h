/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/io/FileSystem.h>
#include <cvt/gfx/Image.h>
#include <string>

namespace cvt {
	namespace FloFile {
		void FloReadFile( Image& flow, std::string const& filename, bool zerounknown = false );
		void FloWriteFile( const Image& img, std::string const & filename );
	}
}
