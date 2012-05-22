#ifndef SLAM_GUI_H
#define SLAM_GUI_H

#include <cvt/gui/Window.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/Label.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/gui/ParamSetView.h>

#include <cvt/vision/slam/stereo/StereoSLAM.h>

#include "SLAMView.h"

namespace cvt
{
	class SLAMGui : public Window 
	{
		public:
                        SLAMGui( ParamSet& depthInitParams );
			~SLAMGui();

			void setCurrentImage( const Image& img );
			void setNumTrackedFeatures( size_t n );
			void resizeEvent( ResizeEvent* event );
			void updateStereoView( const Image& img );
			void updateCameraPose( const Matrix4f & m );

			void addNextFrameButtonDelegate( const Delegate<void (void)> & d );
			void addSteppingButtonDelegate( const Delegate<void (void)> & d );

			void setStepping( bool value );
			void setFPS( float fps );

			void setSaveButtonDelegate( const Delegate<void (void)> & d );
			void addMapClearDelegate( const Delegate<void (void)> & d ){ _resetMap.clicked.add( d ); }
			
			void mapChanged( const SlamMap & m );

		private:
			ImageView	_currentImg;
			Moveable	_currentMov;
			ImageView	_stereoView;
			Moveable	_stereoMov;
			SLAMView	_slamView;
			Moveable	_slamMov;
			Button		_stepping;
			Button		_nextFrame;
			Label		_fpsLabel;
			Button		_resetCamera;
			Button		_saveMap;
            Button          _resetMap;
            ParamSetView    _depthInitParamsView;
            Moveable        _depthParamsMov;


			float		_imageAspect;
			
			PointSet2d	_trackedPoints;
			
			void setupGui();
	};

        inline SLAMGui::SLAMGui( ParamSet& depthInitParams ) : Window( "SLAMGui" ),
		_currentMov( &_currentImg ),
		_stereoMov( &_stereoView ),
		_slamMov( &_slamView ),
		_stepping( "Step" ),
		_nextFrame( "Next" ),
		_fpsLabel( "FPS:" ),
		_resetCamera( "Set Cam" ),
		_saveMap( "Save Map" ),
		_resetMap( "Clear Map" ),
                _depthInitParamsView( depthInitParams ),
                _depthParamsMov( &_depthInitParamsView ),
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
		_currentMov.setSize( (int)w, 300 );
		_stereoMov.setSize( (int)2*w, 300 );
		_stereoMov.setPosition( 0, 300 );

		_slamMov.setSize( 320, 240 );

                _depthParamsMov.setSize( 320, 300 );
                _depthParamsMov.setTitle( "DepthInit Parameters" );
                addWidget( &_depthParamsMov );

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

		this->addWidget( &_currentMov );
		this->addWidget( &_stereoMov );
		this->addWidget( &_slamMov );
		_slamMov.setTitle( "3D View" );

		_stereoMov.setTitle( "Last KF" );

		setNumTrackedFeatures( 0 );

		setVisible( true );
	}
			
	inline void SLAMGui::resizeEvent( ResizeEvent* event )
	{
		Window::resizeEvent( event );
	}

	inline void SLAMGui::setCurrentImage( const Image& img )
	{
		_currentImg.setImage( img );
		_imageAspect = (float)img.width() / (float)img.height();
	}


	inline void SLAMGui::setNumTrackedFeatures( size_t n )
	{
		String s;
		s.sprintf( "Currently Tracked: %d", n );
		_currentMov.setTitle( s );
	}

	inline void SLAMGui::updateStereoView( const Image& img )
	{
		_stereoView.setImage( img );
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
