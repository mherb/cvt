#ifndef SLAM_GUI_H
#define SLAM_GUI_H

#include <cvt/gui/Window.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gfx/GFXEngineImage.h>

#include "StereoSLAM.h"
#include "SLAMView.h"

namespace cvt
{
	class SLAMGui : public Window 
	{
		public:
			SLAMGui(); 
			~SLAMGui();

			void setCurrentImage( const Image& img );
			void resizeEvent( ResizeEvent* event );
			void updateStereoView( const StereoSLAM::ORBData* orbData );

			void updateCameraPose( const Matrix4f & m );

		private:
			ImageView	_image0;
			ImageView	_stereoView;
			SLAMView	_slamView;
			Moveable	_slamMov;

			float		_imageAspect;
			
			void setupGui();
	};

	inline SLAMGui::SLAMGui() : Window( "SLAMGui" ),
		_slamMov( &_slamView ),
		_imageAspect( 1.5f )
	{
		setupGui();
	}

	inline SLAMGui::~SLAMGui()
	{
	}

	inline void SLAMGui::setupGui()
	{
		setSize( 1000, 600 );
		
		float w = _imageAspect * 300;
		_image0.setSize( (int)w, 300 );
		_stereoView.setSize( (int)2*w, 300 );
		_stereoView.setPosition( 0, 300 );

		_slamMov.setSize( 320, 240 );

		this->addWidget( &_image0 );
		this->addWidget( &_stereoView );
		this->addWidget( &_slamMov );

		setVisible( true );
	}
			
	inline void SLAMGui::resizeEvent( ResizeEvent* event )
	{
		size_t h = event->height() / 2;
		float w = _imageAspect * h;
		_image0.setSize( (int)w, h );
		_stereoView.setSize( (int)2*w, h );
		_stereoView.setPosition( 0, h );
		Window::resizeEvent( event );
	}

	inline void SLAMGui::setCurrentImage( const Image& img )
	{
		_image0.setImage( img );
		_imageAspect = (float)img.width() / (float)img.height();
	}

	inline void SLAMGui::updateStereoView( const StereoSLAM::ORBData* orbData )
	{
		Image tmp( 2*orbData->img0->width(), orbData->img0->height(), IFormat::GRAY_UINT8 );
		Recti rect( 0, 0, orbData->img0->width(), orbData->img0->height() );
		tmp.copyRect( 0, 0, *(orbData->img0), rect );
		tmp.copyRect( orbData->img0->width(), 0, *(orbData->img1), rect );

		Image color;
		tmp.convert( color, IFormat::RGBA_UINT8 );

		{
			GFXEngineImage ge( color );
			GFX g( &ge );
			g.color() = Color::GREEN;
			const ORB * orb = ( orbData->orb0 ); 
			for( size_t i = 0; i < orb->size(); i++ ){
				g.fillRect( (*orb)[ i ].pt.x - 1, (*orb)[ i ].pt.y - 1, 3, 3 );
			}
			
			orb = orbData->orb1; 
			for( size_t i = 0; i < orb->size(); i++ ){
				g.fillRect( (*orb)[ i ].pt.x - 1 + orbData->img0->width(), (*orb)[ i ].pt.y - 1, 3, 3 );
			}
		}

		_stereoView.setImage( color );
	}


	void SLAMGui::updateCameraPose( const Matrix4f & m )
	{
		_slamView.setCamPose( m );
	}
}

#endif
