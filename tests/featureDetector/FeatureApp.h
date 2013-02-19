#ifndef CVT_FEATURE_APP_H
#define CVT_FEATURE_APP_H

#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/io/VideoInput.h>
#include <cvt/util/Time.h>
#include <cvt/gui/TimeoutHandler.h>

#include <cvt/vision/features/FAST.h>
#include <cvt/vision/features/Harris.h>
#include <cvt/vision/features/BRIEF.h>
#include <cvt/vision/features/ORB.h>

#include "FeatureView.h"

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
			FeatureView	_view;
			VideoInput&	_video;
			FeatureDetector* _detector;
			FeatureDescriptorExtractor* _oldset;

			Time		_time;
			double		_fps;
			size_t		_iter;

			double		_avgDetectorTime;
			size_t		_allIter;

			Image		_gray;
	};
}

#endif
