/*
			CVT - Computer Vision Tools Library

	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
	PARTICULAR PURPOSE.
 */
#ifndef CVT_HARRIS_H
#define CVT_HARRIS_H

#include <cvt/vision/features/FeatureDetector.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/BoxFilter.h>
#include <cvt/gfx/IMapScoped.h>


namespace cvt
{
	class Harris : public FeatureDetector
	{
		public:
			Harris( float threshold = 1e-6f, size_t border = 3 );
			~Harris();

			void detect( FeatureSet& features, const Image& image );
			void detect( FeatureSet& features, const ImagePyramid& image );

			void setThreshold( float threshold ) { _threshold = threshold; }
			float threshold() const	{ return _threshold; }

			void setBorder( size_t border )	{ _border = border; }
			size_t border() const	{ return _border; }

		private:
			void detectFloat( FeatureSet& features, const Image& image );
			void detectU8( FeatureSet& features, const Image& image );

			float		_threshold;
            size_t		_border;

			static const float  _kappa = 0.04; // 0.04 to 0.15 - TODO: make parameter
			static const int	_radius = 3;
	};

	inline Harris::Harris( float threshold, size_t border ) :
		_threshold( threshold ),
		_border( border )
	{
	}

	inline Harris::~Harris()
	{
	}

	inline void Harris::detect( FeatureSet& features, const Image& image )
	{
		if( image.format() == IFormat::GRAY_FLOAT )
			detectFloat( features, image );
		else if( image.format() == IFormat::GRAY_UINT8 )
			detectU8( features, image );
		else
			throw CVTException( "Input Image format must be GRAY_FLOAT or GRAY_UINT8" );

	}

	inline void Harris::detectFloat( FeatureSet& features, const Image& image )
	{
		size_t w, h, xend;

		w = image.width();
		h = image.height();

		xend = w - _border;

		Image dx( w, h, IFormat::GRAY_FLOAT );
		Image dy( w, h, IFormat::GRAY_FLOAT );
		Image dxy( w, h, IFormat::GRAY_FLOAT );

		image.convolve( dx, IKernel::HAAR_HORIZONTAL_3 );
		image.convolve( dy, IKernel::HAAR_VERTICAL_3 );

		dxy = dx;
		dxy.mul( dy );
		dx.mul( dx );
		dy.mul( dy );

		dx.boxfilter( dx, _radius );
		dy.boxfilter( dy, _radius );
		dxy.boxfilter( dxy, _radius );

		Image tr = dx;
		tr.add( dy );
		tr.mul( tr );

		dxy.mul( dxy );

		dx.mul( dy );
		dx.sub( dxy );
		dx.mad( tr, -0.10f );

		IMapScoped<const float> score( dx );
		size_t yend = h - _border;
		score.setLine( _border );
		for( size_t y = _border; y < yend; y++ ) {
			const float* ptr = score.ptr();
			for( size_t x = _border;  x < xend; x++ ) {
				if( ptr[ x ] > _threshold  )
					features.add( Feature( x, y, 0, 0, ptr[ x ] ) );
			}
			score++;
		}
	}

	inline void Harris::detectU8( FeatureSet& features, const Image& image )
	{
	}

	inline void Harris::detect( FeatureSet& features, const ImagePyramid& image )
	{
		// fuck this
	}

}

#endif
