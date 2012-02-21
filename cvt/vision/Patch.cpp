/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/vision/Patch.h>

namespace cvt {
    
    Patch::Patch( const Image & img, const Recti & rect, PatchFlags flags ) : 
        _computeFlags( flags ),
        _patchData( rect.width, rect.height, img.format() )
    {
        this->update( img, rect );
    }
    
    Patch::~Patch()
    {
    }
    
    void Patch::update( const Image & img, const Recti & rect )
    {
        _patchData.copyRect( 0, 0, img, rect );
        
        if( _computeFlags & NORMALIZE ){
            calcMeanAndVariance();
        }
    }
    
    float Patch::mean() const
    {
        return _mean;
    }
    
    float Patch::variance() const
    {
        return _var;
    }
    
    void Patch::calcMeanAndVariance()
    {
        size_t stride;
        switch ( _patchData.format().type ) {
            case IFORMAT_TYPE_FLOAT:
            {
                float * ptr = _patchData.map<float>( &stride );
                this->meanAndVariance<float>( ptr, stride );
                _patchData.unmap( ptr );
            }
                break;
            case IFORMAT_TYPE_UINT8:
            {
                uint8_t * ptr = _patchData.map<uint8_t>( &stride );
                this->meanAndVariance<uint8_t>( ptr, stride );
                _patchData.unmap( ptr );
            }
                break;
            case IFORMAT_TYPE_UINT16:
            {
                uint16_t * ptr = _patchData.map<uint16_t>( &stride );
                this->meanAndVariance( ptr, stride );
                _patchData.unmap( ptr );
            }
                break;    
            default:
                throw CVTException("MEAN AND STDDEV CALCULATION NOT IMPLEMENTED FOR GIVEN TYPE");
        }
    }
    
    void Patch::dump() const
    {
        switch ( _patchData.format().type ) {
            case IFORMAT_TYPE_UINT8:
            {
                size_t s;
                const uint8_t * ptr = _patchData.map( &s );
                for( size_t y = 0; y < height(); y++ ){
                    for ( size_t x = 0; x < width(); x++) {
                        std::cout <<  (int)ptr[ y * s + x ] << " ";
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
                _patchData.unmap( ptr );
            }
                break;
                
            default:
                throw CVTException( "dump not  implemented for format type" );
                break;
        }
    }
    
}