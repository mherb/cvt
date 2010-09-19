#include "io/FileSystem.h"
#include "gfx/Image.h"
#include <string>

namespace cvt {
	namespace FloFile {
		void FloReadFile( Image& flow, std::string const& filename, bool zerounknown = false );
		void FloWriteFile( const Image& img, std::string const & filename );
	}
}
