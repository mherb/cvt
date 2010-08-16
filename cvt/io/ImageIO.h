#ifndef IMAGEIO_H
#define IMAGEIO_H

#include "gfx/Image.h"

namespace cvt {
    namespace ImageIO {
	void loadPNG( Image& img, std::string path );
	void savePNG( const Image& img, std::string path );
    }
}


#endif
