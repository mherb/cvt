#ifndef CVT_KLT_WINDOW_H
#define CVT_KLT_WINDOW_H

#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/Label.h>
#include <cvt/gui/Slider.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>

#include <cvt/vision/FAST.h>
#include <cvt/vision/ImagePyramid.h>
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

#define PATCH_SIZE ( 8 )

namespace cvt {

	class KLTWindow : public TimeoutHandler
	{
		public:
			typedef GA2<float>			  PoseType;
			//typedef Sim2<float>			  PoseType;
			//typedef Translation2D<float>	  PoseType;
			typedef KLTTracker<PoseType, PATCH_SIZE>  KLTType;	
			typedef KLTType::KLTPType		  KLTPType;

			KLTWindow( VideoInput & video );
			~KLTWindow();

			void onTimeout();

			void ssdThresholdChanged( float val );

			void steppingPressed();
			void nextPressed();

		private:
			uint32_t					_timerId;
			Window						_window;
			ImageView					_imView;

			Label						_ssdSliderLabel;
			Slider<float>				_ssdSlider;
			
			Button						_stepButton;
			Button						_nextButton;
			bool						_stepping;
			bool						_next;

			VideoInput &				_video;

			KLTType						_klt;
			Time						_kltTime;
			double						_kltTimeSum;
			size_t						_kltIters;

			std::vector<KLTPType*>		_patches;
			Time						_time;
			double						_fps;
			size_t						_iter;
			ImagePyramid				_pyramid;

			size_t						_redetectThreshold;

			FAST						_fast;
			FeatureFilter				_gridFilter;

			void drawFeatures( Image & img );
			void redetectFeatures( const Image & img );
			void redetectMultiScale();

			Image						_patchImage;

			/* create an image with the patches inside */
			void createPatchImage( const std::vector<KLTPType*> & patches );

			/* returns number of lost features */
			size_t trackSingleScale( const Image& img );

	};

}

#endif
