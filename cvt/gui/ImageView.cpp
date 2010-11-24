#include <cvt/gui/ImageView.h>
#include <cvt/gfx/IConvert.h>

namespace cvt {
	ImageView::ImageView() : _img( 1, 1, IFormat::RGBA_UINT8, IALLOCATOR_GL )
	{
	}

	ImageView::~ImageView()
	{
	}

	void ImageView::paintEvent( PaintEvent* ev, GFX* g )
	{
		int w, h;
		size( w, h );
		g->drawImage( 0, 0, w, h, _img );
	}

	void ImageView::setImage( const Image& img )
	{
		_img.reallocate( img.width(), img.height(), IFormat::RGBA_UINT8, IALLOCATOR_GL );
		try {
			IConvert::convert( _img, img );
		} catch( Exception e ) {
		}
		update();
	}
}
