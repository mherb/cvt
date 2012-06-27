/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_IMAGE_PYRAMID_H
#define CVT_IMAGE_PYRAMID_H

#include <cvt/gfx/Image.h>
#include <cvt/gfx/IScaleFilter.h>

namespace cvt
{
    class ImagePyramid
    {
        public:
            ImagePyramid( size_t octaves, float scaleFactor );

            /**
             * \brief operators to access the scale space images
             */
            Image&		 operator[]( size_t octave )	   { return _image[ octave ]; }
            const Image& operator[]( size_t octave ) const { return _image[ octave ]; }

            /**
             * \brief update the pyramid: pyr[ 0 ] = image; other scales will be computed from that
             * \param img the zeroth scale image
             */
            void update( const Image& img );

            /**
             * \brief	returns number of octaves in the pyramid
             * \desc	we start counting at 0 for the highest (biggest) image
             */
            size_t octaves() const { return _image.size(); }

            /**
             * \brief	returns the scalefactor between the octaves
             */
            float scaleFactor() const { return _scaleFactor; }

            /**
             *	\brief	set the scalefactor
             */
            void setScaleFactor( float scale ) { _scaleFactor = scale; }

            /**
             *	\brief	reset the number of octaves of the pyramid
             *	\param	o	the new number of octaves
             */
            void setNumOctaves( size_t o );

        private:
            std::vector<Image>	 _image;
            float				 _scaleFactor;
            const IScaleFilterBilinear  _filter;

            /* recompute the scale space from the first octave */
            void recompute();
    };

    inline ImagePyramid::ImagePyramid( size_t octaves, float scaleFactor ) :
        _scaleFactor( scaleFactor )
    {
        setNumOctaves( octaves );
    }

    inline void ImagePyramid::update( const Image& img )
    {
        _image[ 0 ].reallocate( img );
        _image[ 0 ] = img;
        recompute();
    }

    inline void ImagePyramid::recompute()
    {
        float w = _image[ 0 ].width();
        float h = _image[ 0 ].height();

        for( size_t i = 1; i < _image.size(); i++ ){
            w *= _scaleFactor;
            h *= _scaleFactor;
            _image[ i - 1].scale( _image[ i ], ( size_t )w, ( size_t )h, _filter );
        }
    }

    inline void ImagePyramid::setNumOctaves( size_t o )
    {
        if( _image.size() == o )
            return;

        _image.resize( o );
    }

}

#endif
