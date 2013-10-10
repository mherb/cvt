/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/gui/ImageView.h>
#include <cvt/gfx/IConvert.h>

namespace cvt {
	ImageView::ImageView() :
		_alpha( 1.0f ),
		_img( 1, 1, IFormat::BGRA_UINT8, IALLOCATOR_GL ),
		_fixAspect( true )
	{
	}

	ImageView::~ImageView()
	{
	}

	void ImageView::setFixAspect( bool val )
	{
		_fixAspect = val;
	}

	void ImageView::paintEvent( PaintEvent&, GFX& g )
	{
		int w, h;
		size( w, h );
		g.color().setAlpha( _alpha );
		if( _fixAspect ){
			int iw = _img.width();
			int ih = _img.height();
			if( iw > ih ){
				float aspect = ( float )ih / ( float )iw;
				int hnew = aspect * w;
				if( hnew > h ){
					w = h / aspect;
				} else {
					h = hnew;
				}
			}
		}
		g.drawImage( 0, 0, w, h, _img );
	}

	void ImageView::setImage( const Image& img )
	{
		try {
			if( img.format() == IFormat::BAYER_RGGB_UINT8 ||
				img.format() == IFormat::BAYER_GBRG_UINT8 ||
				img.format() == IFormat::BAYER_GRBG_UINT8 ||
				img.format() == IFormat::YUYV_UINT8 ||
				img.format() == IFormat::UYVY_UINT8 ) {
				_img.reallocate( img.width(), img.height(), IFormat::RGBA_UINT8, IALLOCATOR_GL );
				img.convert( _img );
			} else {
				_img.reallocate( img.width(), img.height(), img.format(), IALLOCATOR_GL );
				_img = img;
			}
		} catch( Exception e ) {
			std::cerr << "Copy error: " << e.what() << std::endl;
		}
		update();
	}
}
