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
	ImageView::ImageView() : _alpha( 1.0f ), _img( 1, 1, IFormat::BGRA_UINT8, IALLOCATOR_GL )
	{
	}

	ImageView::~ImageView()
	{
	}

	void ImageView::paintEvent( PaintEvent&, GFX& g )
	{
		int w, h;
		size( w, h );
		g.color().setAlpha( _alpha );
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
