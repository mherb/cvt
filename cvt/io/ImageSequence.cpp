#include <cvt/io/ImageSequence.h>
#include <sstream>
#include <iomanip>

namespace cvt {
    
    ImageSequence::ImageSequence( const std::string & base,
                                  const std::string & ext,
                                  size_t startIndex, 
                                  size_t stopIndex, 
                                  size_t fieldWidth,
                                  char fillChar ) : 
        _loadFunc( 0 ), _baseName( base ), 
        _extension( ext ), _index( startIndex ), 
        _lastIndex( stopIndex ), _fieldWidth( fieldWidth ), _fillChar( fillChar )
    {
        if( _extension == "png" ){
            _loadFunc = &ImageIO::loadPNG;
        } else if( _extension == "cvtraw" ){
            _loadFunc = &ImageIO::loadRAW;
        } else {
            throw CVTException( "Do not know how to load given extension!" );
        }
        
        // load the first frame, such that subsequent calls to 
        // the image information functions work correctly!
        std::stringstream ss;
        ss << _baseName 
           << std::setw( fieldWidth ) << std::setfill( _fillChar ) << _index 
           << "." << _extension;
        
        std::cout << ss.str() << std::endl;
        _loadFunc( _current, ss.str() );
    }
    
    void ImageSequence::nextFrame()
    {
        // build the string and load the frame
        std::stringstream ss;
        ss  << _baseName 
            << std::setw( _fieldWidth ) << std::setfill( _fillChar ) << _index 
            << "." << _extension;
        
        _loadFunc( _current, ss.str() );
        
        if( _index < _lastIndex )
            _index++;
    }
    
}