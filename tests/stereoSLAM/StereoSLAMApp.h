#ifndef STEREO_SLAM_GUI_H
#define STEREO_SLAM_GUI_H

#include <vector>
#include <cvt/io/VideoInput.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/util/Time.h>

#include "StereoSLAM.h"
#include "SLAMGui.h"

namespace cvt
{
	class StereoSLAMApp : public TimeoutHandler
	{
		public:
			StereoSLAMApp( const std::vector<VideoInput*> & cams,
						   const CameraCalibration & c0,
						   const CameraCalibration & c1 );

			~StereoSLAMApp();

			void onTimeout();

			void toggleStepping();
			void nextPressed() { _nextImage = true; };

		private:
			std::vector<VideoInput*> _cams;
			StereoSLAM				 _slam;
			Image					 _img0, _img1;

			uint32_t				_timerId;

			// the gui
			SLAMGui					_gui;

			Time					_time;
			size_t					_timeIter;

			bool					_stepping;
			bool					_nextImage;
	};

	inline StereoSLAMApp::StereoSLAMApp( const std::vector<VideoInput*> & cams,
										 const CameraCalibration & c0,
										 const CameraCalibration & c1 ):
		_cams( cams ),
		_slam( c0, cams[ 0 ]->width(), cams[ 0 ]->height(), c1, cams[ 1 ]->width(), cams[ 1 ]->height() ),
		_img0( cams[ 0 ]->width(), cams[ 0 ]->height(), cams[ 0 ]->format() ),
		_img1( cams[ 1 ]->width(), cams[ 1 ]->height(), cams[ 1 ]->format() ),
		_stepping( true ),
		_nextImage( true )
	{
		_timerId = Application::registerTimer( 5, this );

		Delegate<void ( const StereoSLAM::ORBData* )> d( &_gui, &SLAMGui::updateStereoView );
		_slam.newORBData.add( d );

		Delegate<void ( const Matrix4f& )> d1( &_gui, &SLAMGui::updateCameraPose );
		_slam.newCameraPose.add( d1 );

		Delegate<void ( const std::vector<Vector4f>& )> pointsDel( &_gui, &SLAMGui::addPoints );
		_slam.newPoints.add( pointsDel );
		
		Delegate<void (void)> d2( this, &StereoSLAMApp::nextPressed );
		_gui.addNextFrameButtonDelegate( d2 );
		Delegate<void (void)> d3( this, &StereoSLAMApp::toggleStepping );
		_gui.addSteppingButtonDelegate( d3 );

		_gui.setStepping( _stepping );
	}

	inline StereoSLAMApp::~StereoSLAMApp()
	{
		Application::unregisterTimer( _timerId );
		for( size_t i = 0; i < _cams.size(); i++ ){
			delete _cams[ i ];
		}		
	}

	inline void StereoSLAMApp::onTimeout()
	{
		if( !_stepping || ( _stepping && _nextImage ) ){
			_cams[ 0 ]->nextFrame();
			_cams[ 1 ]->nextFrame();
			_nextImage = false;
		}

		_cams[ 0 ]->frame().convert( _img0, IFormat::GRAY_UINT8 );
		_cams[ 1 ]->frame().convert( _img1, IFormat::GRAY_UINT8 );
		_slam.newImages( _img0, _img1 );
		
		_gui.setCurrentImage( _slam.undistorted( 0 ) );

		_timeIter++;
		if( _time.elapsedSeconds() > 3.0f ){
			_gui.setFPS( 1000.0f * _timeIter / _time.elapsedMilliSeconds() );
			_time.reset();
			_timeIter = 0;
		}
	}


	inline void StereoSLAMApp::toggleStepping() 
	{ 
		_stepping = !_stepping; 
		_gui.setStepping( _stepping );
	}

}

#endif
