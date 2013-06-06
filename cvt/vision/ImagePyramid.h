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
            void update( const Image& img, const IScaleFilter& sfilter = IScaleFilterGauss() );

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

            void save( const String& basename ) const;

            template <class Func>
            void apply( ImagePyramid& out, const Func& f ) const;

            void convolve( ImagePyramid& out, const IKernel& kernel ) const;
            void convolve( ImagePyramid& out, const IKernel& hkernel, const IKernel& vkernel ) const;
            void convert( ImagePyramid& out, const IFormat& dstFormat ) const;
            void integralImage( ImagePyramid& out ) const;

        private:
            std::vector<Image>       _image;
            float                    _scaleFactor;

            /* recompute the scale space from the first octave */
            void recompute( const IScaleFilter &sfilter );
    };

    inline ImagePyramid::ImagePyramid( size_t octaves, float scaleFactor ) :
        _scaleFactor( scaleFactor )
    {
        _image.resize( octaves );
    }

    inline void ImagePyramid::update( const Image& img, const IScaleFilter& sfilter )
    {
        _image[ 0 ].reallocate( img );
        _image[ 0 ] = img;
        recompute( sfilter );
    }

    inline void ImagePyramid::recompute(  const IScaleFilter& sfilter )
    {
        float w = _image[ 0 ].width();
        float h = _image[ 0 ].height();

        for( size_t i = 1; i < _image.size(); i++ ){
            w *= _scaleFactor;
            h *= _scaleFactor;
            _image[ i - 1 ].scale( _image[ i ], ( size_t )w, ( size_t )h, sfilter );
        }
    }

    inline void ImagePyramid::save( const String& basename ) const
    {
        String base, ext;
        ssize_t dpos = basename.rfind( '.' );
        base = basename.substring( 0, dpos );
        ext = basename.substring( dpos, basename.length() - dpos );

        String filename;
        for( size_t i = 0; i < _image.size(); i++ ){
            filename.sprintf( "%s_octave_%02d.%s", base.c_str(), i, ext.c_str() );
            _image[ i ].save( filename );
        }
    }

    template <class Func>
    inline void ImagePyramid::apply( ImagePyramid& out, const Func& f ) const
    {
        for( size_t i = 0; i < _image.size(); i++ ){
            f( _image[ i ], out[ i ] );
        }
    }

    inline void ImagePyramid::convolve( ImagePyramid& out, const IKernel& kernel ) const
    {
        for( size_t i = 0; i < _image.size(); i++ ){
            out[ i ].reallocate( _image[ i ] );
            _image[ i ].convolve( out[ i ], kernel );
        }
    }

    inline void ImagePyramid::convolve( ImagePyramid& out, const IKernel& hkernel, const IKernel& vkernel ) const
    {
        for( size_t i = 0; i < _image.size(); i++ ){
            out[ i ].reallocate( _image[ i ] );
            _image[ i ].convolve( out[ i ], hkernel, vkernel );
        }
    }

    inline void ImagePyramid::convert( ImagePyramid& out, const IFormat& dstFormat ) const
    {
        for( size_t i = 0; i < _image.size(); i++ ){
            out[ i ].reallocate( _image[ i ].width(), _image[ i ].height(), dstFormat, _image[ i ].memType() );
            _image[ i ].convert( out[ i ], dstFormat );
        }
    }

    inline void ImagePyramid::integralImage( ImagePyramid& out ) const
    {
        for( size_t i = 0; i < _image.size(); i++ ){
            _image[ i ].integralImage( out[ i ] );
        }
    }

}

#endif
