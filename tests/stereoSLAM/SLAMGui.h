#ifndef SLAM_GUI_H
#define SLAM_GUI_H

#include <cvt/gui/Window.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/Label.h>
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

			void addNextFrameButtonDelegate( const Delegate<void (void)> & d );
			void addSteppingButtonDelegate( const Delegate<void (void)> & d );

			void setStepping( bool value );
			void setFPS( float fps );

			void updateTrackedPoints( const PointSet2d & pset );

			void setSaveButtonDelegate( const Delegate<void (void)> & d );
			void addMapClearDelegate( const Delegate<void (void)> & d ){ _resetMap.clicked.add( d ); }
			
			void mapChanged( const SlamMap & m );


		private:
			ImageView	_image0;
			ImageView	_stereoView;
			SLAMView	_slamView;
			Moveable	_slamMov;
			Button		_stepping;
			Button		_nextFrame;
			Label		_fpsLabel;
			Button		_resetCamera;
			Button		_saveMap;
			Button		_resetMap;

			float		_imageAspect;
			
			PointSet2d	_trackedPoints;
			
			void setupGui();
	};

	inline SLAMGui::SLAMGui() : Window( "SLAMGui" ),
		_slamMov( &_slamView ),
		_stepping( "Step" ),
		_nextFrame( "Next" ),
		_fpsLabel( "FPS:" ),
		_resetCamera( "Set Cam" ),
		_saveMap( "Save Map" ),
		_resetMap( "Clear Map" ),
		_imageAspect( 1.5f )
	{
		setupGui();
		
		Delegate<void (void)> d( &_slamView, &SLAMView::resetCameraView );
		_resetCamera.clicked.add( d );
		
		Delegate<void (void)> resetMap( &_slamView, &SLAMView::clearMap );
		addMapClearDelegate( resetMap );
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

		// buttons to the top right
		WidgetLayout wl;
		wl.setAnchoredRight( 10, 100 );
		wl.setAnchoredTop( 10, 20 );
		this->addWidget( &_stepping, wl );
		wl.setAnchoredTop( 40, 20 );
		this->addWidget( &_nextFrame, wl );
		wl.setAnchoredTop( 70, 20 );
		this->addWidget( &_fpsLabel, wl );
		wl.setAnchoredTop( 100, 20 );
		this->addWidget( &_resetCamera, wl );
		wl.setAnchoredTop( 130, 20 );
		this->addWidget( &_saveMap, wl );
		wl.setAnchoredTop( 160, 20 );
		this->addWidget( &_resetMap, wl );

		this->addWidget( &_image0 );
		this->addWidget( &_stereoView );
		this->addWidget( &_slamMov );
		_slamMov.setTitle( "3D View" );

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
		Image color;
		img.convert( color, IFormat::RGBA_UINT8 );

		{
			GFXEngineImage ge( color );
			GFX g( &ge );
			g.color() = Color::GREEN;

			Recti r;
			r.width = 31;
			r.height = 31;
			for( size_t i = 0; i < _trackedPoints.size(); i++ ){
				r.x = ( int )_trackedPoints[ i ].x - 15;
				r.y = ( int )_trackedPoints[ i ].y - 15;
				g.drawRect( r );
			}
		}

		_image0.setImage( color );
		_imageAspect = (float)img.width() / (float)img.height();
	}

	inline void SLAMGui::updateStereoView( const StereoSLAM::ORBData* orbData )
	{
		Image tmp( 2*orbData->img0->width(), orbData->img0->height(), IFormat::GRAY_UINT8 );
		Recti rect( 0, 0, orbData->img0->width(), orbData->img0->height() );
		tmp.copyRect( 0, 0, *(orbData->img0), rect );
		tmp.copyRect( orbData->img0->width(), 0, *(orbData->img1), rect );

		std::vector<FeatureMatch> * matches = orbData->matches;

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

			g.color() = Color::RED;
			for( size_t i = 0; i < matches->size(); i++ ){
				if( (*matches)[ i ].feature1 ){
					Vector2f p2 = (*matches)[ i ].feature1->pt;
					p2.x += orbData->img0->width();
					g.drawLine( (*matches)[ i ].feature0->pt, p2 );
				}
			}
		}

		_stereoView.setImage( color );
	}

	inline void SLAMGui::updateCameraPose( const Matrix4f & m )
	{
		_slamView.setCamPose( m );
	}

	inline void SLAMGui::addNextFrameButtonDelegate( const Delegate<void (void)> & d )
	{
		_nextFrame.clicked.add( d ); 
	}

	inline void SLAMGui::addSteppingButtonDelegate( const Delegate<void (void)> & d )
	{
		_stepping.clicked.add( d ); 
	}

	inline void SLAMGui::setStepping( bool value )
	{
		if( value ){
			_stepping.setLabel( "RUN" );
		} else {
			_stepping.setLabel( "STEP" );
		}
	}

	inline void SLAMGui::setFPS( float fps )
	{
		String title;
		title.sprintf( "FPS: %0.1f", fps );
		_fpsLabel.setLabel( title );
	}

	inline void SLAMGui::updateTrackedPoints( const PointSet2d & pset )
	{
		_trackedPoints = pset;
	}
			
	inline void SLAMGui::setSaveButtonDelegate( const Delegate<void (void)> & d )
	{
		_saveMap.clicked.add( d );
	}
			
	inline void SLAMGui::mapChanged( const SlamMap & m )
	{
		_slamView.mapChanged( m );
	}
}

#endif
