#ifndef STEREO_SLAM_GUI_H
#define STEREO_SLAM_GUI_H

#include <vector>
#include <cvt/io/VideoInput.h>
#include <cvt/gui/TimeoutHandler.h>

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
	};

	inline StereoSLAMApp::StereoSLAMApp( const std::vector<VideoInput*> & cams,
										 const CameraCalibration & c0,
										 const CameraCalibration & c1 ):
		_cams( cams ),
		_slam( c0, cams[ 0 ]->width(), cams[ 0 ]->height(), c1, cams[ 1 ]->width(), cams[ 1 ]->height() ),
		_img0( cams[ 0 ]->width(), cams[ 0 ]->height(), cams[ 0 ]->format() ),
		_img1( cams[ 1 ]->width(), cams[ 1 ]->height(), cams[ 1 ]->format() )
	{
		_timerId = Application::registerTimer( 1, this );
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
		std::cout << "Timeout" << std::endl;
		_cams[ 0 ]->nextFrame();
		_cams[ 1 ]->nextFrame();
		_img0 = _cams[ 0 ]->frame();
		_img1 = _cams[ 1 ]->frame();
		
		_gui.setCurrentImage( _img0 );
		_gui.setSecondImage( _img0 );

		_slam.newImages( _img0, _img1 );
	}
}

#endif
