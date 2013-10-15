#ifndef CVT_IMAGESEQUENCE_H
#define CVT_IMAGESEQUENCE_H

#include <cvt/io/VideoInput.h>
#include <cvt/util/String.h>
#include <vector>

namespace cvt
{
    class ImageSequence : public VideoInput
    {
        public:
            ImageSequence( const String& basename,
                           const String& ext );
        
            ~ImageSequence(){}
        
            size_t  width() const { return _current.width(); }
            size_t  height() const { return _current.height(); }
            const   IFormat & format() const { return _current.format(); }
            const   Image & frame() const { return _current; }
            bool    nextFrame( size_t timeout = 0 );
			bool	hasNext() const;
            
        
        private:
            Image					_current;   
			std::vector<String>		_files;
            size_t					_index;
	
			bool extractFolder( String& folder, const String& basename ) const;
    };
}

#endif
