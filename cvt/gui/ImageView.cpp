#include <cvt/gui/ImageView.h>
#include <cvt/gfx/IConvert.h>

namespace cvt {
	ImageView::ImageView() : _alpha( 1.0f ), _img( 1, 1, IFormat::BGRA_UINT8, IALLOCATOR_GL )
	{
	}

	ImageView::~ImageView()
	{
	}

	void ImageView::paintEvent( PaintEvent*, GFX* g )
	{
		int w, h;
		size( w, h );
		g->color().setAlpha( _alpha );
		g->drawImage( 0, 0, w, h, _img );
	}

	void ImageView::setImage( const Image& img )
	{
		_img.reallocate( img.width(), img.height(), IFormat::BGRA_UINT8, IALLOCATOR_GL );
		try {
			IConvert::convert( _img, img );
		} catch( Exception e ) {
			std::cerr << "Conversion error: " << e.what() << std::endl;
		}
		update();
	}
}
