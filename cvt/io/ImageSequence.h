#ifndef CVT_IMAGESEQUENCE_H
#define CVT_IMAGESEQUENCE_H

#include <cvt/io/VideoInput.h>
#include <cvt/io/ImageIO.h>

namespace cvt
{
    class ImageSequence : public VideoInput
    {
        public:
            ImageSequence( const std::string & base, 
                           const std::string & ext,
                           size_t startIndex, 
                           size_t stopIndex, 
                           size_t fieldWidth,
                           char fillChar = '0' );
        
            ~ImageSequence(){}
        
            size_t  width() const { return _current.width(); }
            size_t  height() const { return _current.height(); }
            const   IFormat & format() const { return _current.format(); }
            const   Image & frame() const { return _current; }
            void    nextFrame();
            
        
        private:
            void ( *_loadFunc )( Image &, const std::string & );
        
            Image       _current;   
            std::string _baseName;
            std::string _extension;            
            size_t      _index;
            size_t      _lastIndex;
            size_t      _fieldWidth;
            char        _fillChar;
    };
}

#endif