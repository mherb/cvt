#ifndef CVT_FEATURE_APP_H
#define CVT_FEATURE_APP_H

#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/io/VideoInput.h>
#include <cvt/util/Time.h>
#include <cvt/gui/TimeoutHandler.h>

namespace cvt
{
	class FeatureApp : public TimeoutHandler 
	{
		public:
			FeatureApp( VideoInput & video );
			~FeatureApp();
			void onTimeout();

		private:
			uint32_t	_timerId;
			Window		_window;
			ImageView	_imView;
			VideoInput&	_video;

			Time		_time;
			double		_fps;
			size_t		_iter;

			double		_avgDetectorTime;
			size_t		_allIter;

			Image		_gray;
	};
}

#endif
