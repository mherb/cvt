#ifndef FILTER_APP_H
#define FILTER_APP_H

#include <cvt/gui/Window.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gfx/IFilter.h>
#include <cvt/io/Camera.h>
#include <cvt/util/ParamSet.h>
#include <cvt/util/Time.h>
#include <cvt/cl/CLContext.h>

class FilterApp : public cvt::TimeoutHandler
{
	public:		
		void onTimeout();
		static int run( const std::string & title );
		
	private:
		FilterApp( const std::string & name );
		~FilterApp();
		void initFilter();
		void initCamera();
		
		cvt::Window				_appWindow;	
		cvt::Button				_quitButton;
		cvt::Delegate<void ()> 	_quitDel;
		cvt::ImageView 			_inImageView;
		cvt::ImageView			_outImageView;
		cvt::Moveable			_mIn, _mOut;
		
		cvt::Camera*			_cam;
		cvt::IFilter*			_filter;
		cvt::IFilterType		_filterType;
		cvt::ParamSet*			_params;
		uint32_t				_frames;
		cvt::Time				_timer;
		
		cvt::CLContext			_cl;
		cvt::Image				_in;		
		cvt::Image				_out;
		
		uint32_t 				_timerId;
		size_t					_inputHandle; // handle to set the input Image in the paramset!
		size_t					_outputHandle;
};

#endif