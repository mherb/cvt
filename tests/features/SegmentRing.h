//
//  SegmentRing.h
//  CVTools
//
//  Created by Sebastian Klose on 15.02.11.
//  Copyright 2011. All rights reserved.
//
#ifndef CVT_SEGMENT_RING_H
#define CVT_SEGMENT_RING_H

#include <stdlib.h>

namespace cvt {
    // representation of the AST Bresenham circle around a feature point
    class SegmentRing
    {
        public:
            /* center of the circle, stride of the underlying image, and brightness difference threshold */
            SegmentRing( const uint8_t * center, size_t stride, uint8_t t );
            ~SegmentRing(){ free( _pixels ); }
        
        private:
            uint8_t * _pixels;
            bool      _isBrighterRing;
    };
    
    inline SegmentRing::SegmentRing( const uint8_t * center, size_t stride, uint8_t t ) : _pixels( 0 )
    {
        posix_memalign( &_pixels, 16, 16 );
                        
        uint8_t * c;
        // collect the 16 ring pixels:
        c = center - 3 * stride - 1;
        _pixels[ 0 ]    = *c++;
        _pixels[ 1 ]    = *c++; 
        _pixels[ 2 ]    = *c;
        
        c = c + stride - 3;
        _pixels[ 15 ]   = *c;
        _pixels[ 3 ]    = *( c + 4 );
        
        c = c + stride - 1;
        _pixels[ 14 ]   = *c;
        _pixels[ 4 ]    = *( c + 6 );
        
        c = c + stride;
        _pixels[ 13 ]   = *c;
        _pixels[ 5 ]    = *( c + 6 );
        
        c = c + stride;
        _pixels[ 12 ]   = *c;
        _pixels[ 6 ]    = *( c + 6 );
        
        c = c + stride + 1;
        _pixels[ 11 ]   = *c;
        _pixels[ 7 ]    = *( c + 4 );
        
        c = c + stride + 1;
        _pixels[ 10 ] = *c++;
        _pixels[ 9 ]  = *c++;
        _pixels[ 8 ]  = *c;
        
        c = _pixels;
        uint8_t * s, *e;
        size_t len = 0;
        
        bool darker = true;
        s = c; e = c;
        for( size_t i = 0; i < 16; i++ ){
            if( c < ( int )p - t ){
                // brighter
                if( darker ){
                    darker = false;                    
                } else {
                    darker = true;
                }
            } else {
                if( c < ( p + t ) ){
                    // darker
                }
            }
        }
    }
    
}

#endif