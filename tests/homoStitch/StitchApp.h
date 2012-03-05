#ifndef CVT_STITCH_APP_H
#define CVT_STITCH_APP_H

#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/io/VideoInput.h>
#include <cvt/util/Time.h>
#include <cvt/gui/TimeoutHandler.h>

namespace cvt
{
	class StitchApp : public TimeoutHandler
	{
		public:
			StitchApp( Camera* cam, const Matrix3f& calib );
			~StitchApp();

			void onTimeout();

		private:
			Camera* _cam;
			uint32_t	_timerId;
			Window		_window;
			ImageView	_imView;
			HomographyStitching _stitcher;

	};

	inline StitchApp::StitchApp( Camera* cam, const Matrix3f& calib ) :
		_cam( cam ),
		_window( "Stitching" ),
		_stitcher( calib )
	{
		_timerId = Application::registerTimer( 20, this );
		_window.setSize( 640, 480 );
		WidgetLayout wl;
		wl.setAnchoredTopBottom( 0, 0 );
		wl.setAnchoredLeftRight( 0, 0 );
		_window.addWidget( &_imView, wl );
		_window.setVisible( true );
		_window.update();				
	}

	inline StitchApp::~StitchApp()
	{
		Application::unregisterTimer( _timerId );
	}

	inline void StitchApp::onTimeout()
	{
		_cam->nextFrame();

		Image rgb;
		_cam->frame().convert( rgb, IFormat::RGBA_UINT8 );
		_stitcher.addImage( rgb );
		_imView.setImage( _stitcher.stitchedImage() );
	}



}

#endif
