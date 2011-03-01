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
#include <cvt/math/Math.h>

namespace cvt {
    // representation of the AST Bresenham circle around a feature point
    class SegmentRing
    {
        public:
            /* center of the circle, stride of the underlying image, and brightness difference threshold */
            SegmentRing( const uint8_t * center, size_t stride, uint8_t t );
            SegmentRing();
        
            bool update( const uint8_t * center, size_t stride, uint8_t t );
            
            ~SegmentRing(){ 
                free( _pixels ); 
                free( _differences );
            }
        
        private:
            uint8_t * _pixels;
            int16_t * _differences;
            float     _avg;
        
            bool      _isBrighterRing;
            int   _segmentOffset;
    };
    
    inline SegmentRing::SegmentRing()
    {
        posix_memalign( ( void ** )&_pixels, 16, 16 );
        posix_memalign( ( void ** )&_differences, 16, 16 );
    }
    
    inline SegmentRing::SegmentRing( const uint8_t * center, size_t stride, uint8_t t ) : _pixels( 0 )
    {
        posix_memalign( ( void ** )&_pixels, 16, 16 );
        posix_memalign( ( void ** )&_differences, 16, 16 );
        update( center, stride, t );        
    }
    
    inline bool SegmentRing::update( const uint8_t * center, size_t stride, uint8_t t )
    {        
        // collect the 16 ring pixels:
        const uint8_t * c = center;
        _pixels[ 0 ]    = c[ -3*stride ];
        _pixels[ 1 ]    = c[ -3*stride+1 ];
        _pixels[ 2 ]    = c[ -2*stride+2 ];
        _pixels[ 3 ]    = c[ -stride+3 ];
        _pixels[ 4 ]    = c[ 3 ];
        _pixels[ 5 ]    = c[ stride + 3 ];
        _pixels[ 6 ]    = c[ 2*stride + 2 ];        
        _pixels[ 7 ]    = c[ 3*stride + 1 ];
        _pixels[ 8 ]    = c[ 3*stride ];
        _pixels[ 9 ]    = c[ 3*stride - 1 ];
        _pixels[ 10 ]    = c[ 2*stride - 2 ];
        _pixels[ 11 ]    = c[ stride - 3 ];
        _pixels[ 12 ]    = c[ -3 ];
        _pixels[ 13 ]    = c[ -stride -3 ];
        _pixels[ 14 ]    = c[ -2*stride - 2 ];
        _pixels[ 15 ]    = c[ -3*stride - 1 ];
        
        size_t runLength = 0;
        size_t atStart = 0;
        int lower = *c - t;
        int upper = *c + t;
        
        for( size_t i = 0; i < 16; i++ ){
            if( _pixels[ i ] < lower ){
                if( i == atStart ){
                    atStart++;
                }
                runLength++;
            } else {
                runLength = 0;
            }
            if( runLength == 9 ){
                return true;
            }
        }
        if( ( runLength + atStart ) > 8 ){
            return true;
        }
        
        atStart = 0;
        runLength = 0;
        for( size_t i = 0; i < 16; i++ ){
            if( _pixels[ i ] > upper ){
                if( i == atStart ){
                    atStart++;
                }
                runLength++;
            } else {
                runLength = 0;
            }
            if( runLength == 9 ){
                return true;
            }
        }
        if( ( runLength + atStart ) > 8 ){
            return true;
        }
        
        std::cout << "Center: " << ( int )*center << " Threshold: " << ( int )t << "\nSegment: ";
        std::cout << "Lower: " << (int)lower << ", Upper: " << (int)upper << std::endl;
        std::cout << "Runlen: " << runLength << ", AtStart: " << atStart << std::endl;
        for( size_t i = 0; i < 16; i++ ){
            std::cout << " [ " << ( int )_pixels[ i ] << " ] ";
        }
        std::cout << std::endl;
        
        return false;
    }
    
}

#endif