#include <cvt/io/FileSystem.h>
#include <cvt/gfx/Image.h>
#include <string>

namespace cvt {
	namespace FloFile {
		void FloReadFile( Image& flow, std::string const& filename, bool zerounknown = false );
		void FloWriteFile( const Image& img, std::string const & filename );
	}
}
