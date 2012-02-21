/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_PATCH_H
#define CVT_PATCH_H

#include <cvt/gfx/Image.h>
#include <cvt/geom/Rect.h>
#include <cvt/util/Flags.h>

namespace cvt {
    
    enum PatchFlagTypes {
        PLAIN       = ( 1 << 0 ),
		NORMALIZE   = ( 1 << 1 )
	};
    
	CVT_ENUM_TO_FLAGS( PatchFlagTypes, PatchFlags )
    
    class Patch
    {
    public:
        Patch( const Image & img, const Recti & rect, PatchFlags computeFlags = PLAIN );
        ~Patch();
        
        void update( const Image & img, const Recti & rect );
        size_t width() const { return _patchData.width(); }
        size_t height() const { return _patchData.height(); }
        const Image & data() const { return _patchData; }
        
        float mean() const;
        float variance() const;
        
        void dump() const;
        
    private:
        PatchFlags  _computeFlags;
        Image       _patchData;
        
        float       _mean;
        float       _var;
        
        void calcMeanAndVariance();
        
        template <typename T> void meanAndVariance( const T* input, size_t stride )
        {
            float sum = 0.0f;
            float sqrSum = 0.0f;
            
            size_t height = _patchData.height();
            size_t width = _patchData.width();
            
            
            while( height-- ){
                for( size_t i = 0; i < width; i++ ){
                    sum += input[ i ];
                    sqrSum += ( float )Math::sqr( ( float )input[ i ] );
                }
                input += stride;
            }
            
            float normalizer = 1.0f / ( float )( width * this->height() );

            _mean = sum * normalizer;
            _var = sqrSum * normalizer - Math::sqr( _mean );
        }
    };
}

#endif