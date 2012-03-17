#ifndef CVT_KLT_WINDOW_H
#define CVT_KLT_WINDOW_H

#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/Label.h>
#include <cvt/gui/Slider.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>

#include <cvt/vision/FAST.h>
#include <cvt/vision/FeatureFilter.h>
#include <cvt/io/RawVideoReader.h>
#include <cvt/io/Resources.h>
#include <cvt/util/Time.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/EigenBridge.h>

#include <cvt/math/Translation2D.h>
#include <cvt/math/Sim2.h>
#include <cvt/math/GA2.h>

#include <cvt/vision/KLTTracker.h>
#include <cvt/gfx/GFXEngineImage.h>

namespace cvt {

	class KLTWindow : public TimeoutHandler
	{
		public:
			//typedef GA2<float>			  PoseType;
			//typedef Sim2<float>			  PoseType;
			typedef Translation2D<float>	  PoseType;
			typedef KLTTracker<PoseType, 16>  KLTType;	
			typedef KLTType::KLTPType		  KLTPType;

			KLTWindow( VideoInput & video );
			~KLTWindow();

			void onTimeout();

		private:
			uint32_t					_timerId;
			Window						_window;
			ImageView					_imView;

			Label						_ssdSliderLabel;
			Slider<float>				_ssdSlider;
			
			VideoInput &				_video;

			KLTType						_klt;
			Time						_kltTime;
			double						_kltTimeSum;
			size_t						_kltIters;

			std::vector<KLTPType*>		_patches;
			std::vector<PoseType>		_poses;
			Time						_time;
			double						_fps;
			size_t						_iter;
			std::vector<Image>			_pyramid;

			FAST						_fast;
			FeatureFilter				_gridFilter;

			void drawFeatures( Image & img );
			void redetectFeatures( const Image & img );

			Vector2f patchToCurrentPosition( const PoseType& pose, const KLTPType& patch ) const;
	};

}

#endif
