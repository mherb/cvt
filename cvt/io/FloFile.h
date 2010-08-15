#include "io/FileSystem.h"
#include "gfx/Image.h"
#include <string>

namespace cvt {
    namespace FloFile {
	void FloReadFile( Image& flow, std::string const& filename );
	void FloWriteFile( Image& img, std::string const & filename );
	float FlowAEE( Image& flow, Image& gt );
	float FlowAAE( Image& flow, Image& gt );
    }
}
