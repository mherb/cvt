/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/io/ImageSequence.h>
#include <sstream>
#include <iomanip>

namespace cvt {
    
    ImageSequence::ImageSequence( const String & base,
                                  const String & ext,
                                  size_t startIndex, 
                                  size_t stopIndex, 
                                  size_t fieldWidth,
                                  char fillChar ) : 
        _baseName( base ), 
        _extension( ext ), _index( startIndex ), 
        _lastIndex( stopIndex ), _fieldWidth( fieldWidth ), _fillChar( fillChar )
    {       
        // load the first frame, such that subsequent calls to 
        // the image information functions work correctly!
        
        std::stringstream ss;
        ss << _baseName 
           << std::setw( fieldWidth ) << std::setfill( _fillChar ) << _index 
           << "." << _extension;
        
        _current.load( ss.str().c_str() );
    }
    
    void ImageSequence::nextFrame()
    {
        // build the string and load the frame
        std::stringstream ss;
        ss  << _baseName 
            << std::setw( _fieldWidth ) << std::setfill( _fillChar ) << _index 
            << "." << _extension;
        
        _current.load( ss.str().c_str() );
        
        if( _index < _lastIndex )
            _index++;
    }
    
}
