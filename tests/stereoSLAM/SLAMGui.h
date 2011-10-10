#ifndef SLAM_GUI_H
#define SLAM_GUI_H

#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>

namespace cvt
{
	class SLAMGui : public Window 
	{
		public:
			SLAMGui(); 
			~SLAMGui();

			void setCurrentImage( const Image& img );
			void setSecondImage( const Image& img );

			void resizeEvent( ResizeEvent* event );

		private:
			ImageView	_image0;
			ImageView	_image1;
			float		_imageAspect;
			
			void setupGui();
	};

	inline SLAMGui::SLAMGui() : Window( "SLAMGui" ),
		_imageAspect( 1.5f )
	{
		setupGui();
	}

	inline SLAMGui::~SLAMGui()
	{
	}

	inline void SLAMGui::setupGui()
	{
		setSize( 800, 600 );
		
		float w = _imageAspect * 300;
		_image0.setSize( (int)w, 300 );
		_image1.setSize( (int)w, 300 );
		_image1.setPosition( 0, 300 );
		this->addWidget( &_image0 );
		this->addWidget( &_image1 );

		setVisible( true );
	}
			
	inline void SLAMGui::resizeEvent( ResizeEvent* event )
	{
		size_t h = event->height() / 2;
		float w = _imageAspect * h;
		_image0.setSize( (int)w, h );
		_image1.setSize( (int)w, h );
		_image1.setPosition( 0, h );
		Window::resizeEvent( event );
	}

	inline void SLAMGui::setCurrentImage( const Image& img )
	{
		_image0.setImage( img );
		_imageAspect = (float)img.width() / (float)img.height();
	}


	inline void SLAMGui::setSecondImage( const Image& img )
	{
		_image1.setImage( img );
	}
}

#endif
