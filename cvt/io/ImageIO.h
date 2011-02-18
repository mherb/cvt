#ifndef IMAGEIO_H
#define IMAGEIO_H

#include "gfx/Image.h"

namespace cvt {
    namespace ImageIO {
        void loadPNG( Image& img, std::string path );
        void savePNG( const Image & img, std::string path );
        
        void loadRAW( Image & img, const std::string & path );
        void saveRAW( const Image & img, const std::string  & path );
    }
}


#endif
