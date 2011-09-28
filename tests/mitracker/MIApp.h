#include <cvt/gui/Window.h>
#include <cvt/gui/Application.h>
#include <cvt/gui/TimeoutHandler.h>

#include "MITracker.h"
#include "MIGui.h"

namespace cvt
{
	class MIApp : public TimeoutHandler
	{
		public:
			MIApp( VideoInput* input );
			~MIApp();

			void onTimeout();

		private:
			VideoInput*	_input;
			MIGui		_gui;
			MITracker	_tracker;
			uint32_t	_timerId;
	};

	inline MIApp::MIApp( VideoInput* input ) :
		_input( input )
	{
		_timerId = Application::registerTimer( 10, this );
	}

	inline MIApp::~MIApp()
	{
		Application::unregisterTimer( _timerId );
	}

	inline void MIApp::onTimeout()
	{
		_input->nextFrame();
		
		// make it gray
		Image gray;
		_input->frame().convert( gray, IFormat::GRAY_UINT8 );
	
		//_tracker.updateInput( gray );

		_tracker.updateTemplate( gray );
		_gui.setTemplateImage( gray, 
							   _tracker.templateGradX(), 
							   _tracker.templateGradY() );

	}


}
