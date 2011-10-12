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

		private:
			std::vector<VideoInput*> _cams;
			StereoSLAM				 _slam;
			Image					 _img0, _img1;

			uint32_t				_timerId;

			// the gui
			SLAMGui					_gui;

			Time					_time;
			size_t					_timeIter;
	};

	inline StereoSLAMApp::StereoSLAMApp( const std::vector<VideoInput*> & cams,
										 const CameraCalibration & c0,
										 const CameraCalibration & c1 ):
		_cams( cams ),
		_slam( c0, cams[ 0 ]->width(), cams[ 0 ]->height(), c1, cams[ 1 ]->width(), cams[ 1 ]->height() ),
		_img0( cams[ 0 ]->width(), cams[ 0 ]->height(), cams[ 0 ]->format() ),
		_img1( cams[ 1 ]->width(), cams[ 1 ]->height(), cams[ 1 ]->format() )
	{
		_timerId = Application::registerTimer( 10, this );

		Delegate<void ( const StereoSLAM::ORBData* )> d( &_gui, &SLAMGui::updateStereoView );
		_slam.newORBData.add( d );
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
		_cams[ 0 ]->nextFrame();
		_cams[ 1 ]->nextFrame();

		_cams[ 0 ]->frame().convert( _img0, IFormat::GRAY_UINT8 );
		_cams[ 1 ]->frame().convert( _img1, IFormat::GRAY_UINT8 );
		_slam.newImages( _img0, _img1 );
		
		_gui.setCurrentImage( _slam.undistorted( 0 ) );
		_timeIter++;

		if( _timeIter == 20 ){
			std::cout << "FPS: " << 1000*_timeIter / _time.elapsedMilliSeconds() << std::endl;
			_time.reset();
			_timeIter = 0;
		}
	}
}

#endif
