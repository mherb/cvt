#include <cvt/gui/Window.h>
#include <cvt/gui/Application.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/util/Time.h>

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

			void selectionDidStart();
			void selectionFinished( const Rectf& r ); 

		private:
			VideoInput*	_input;
			MIGui		_gui;
			MITracker	_tracker;
			uint32_t	_timerId;
			Time		_fpsTime;
			size_t		_iter;
			bool		_selectingTemplate;
			Image		_currGray;
	};

	inline MIApp::MIApp( VideoInput* input ) :
		_input( input ),
		_iter( 0 ),
		_selectingTemplate( true )
	{
		_timerId = Application::registerTimer( 5, this );

		// add the delegates to the gui
		Delegate<void (void)> selStart( this, &MIApp::selectionDidStart );
		Delegate<void (const Rectf&)> selDone( this, &MIApp::selectionFinished );

		_gui.addSelectionDelegates( selStart, selDone );
	}

	inline MIApp::~MIApp()
	{
		Application::unregisterTimer( _timerId );
	}

	inline void MIApp::onTimeout()
	{
		_iter++;

		_input->nextFrame();
		_input->frame().convert( _currGray, IFormat::GRAY_UINT8 );

		if( !_selectingTemplate ){
			_tracker.updateInput( _currGray );
		}

		if( _iter == 50 ){
			_gui.setFPS( _iter * 1000.0f / _fpsTime.elapsedMilliSeconds() );
			_fpsTime.reset();
			_iter = 0;
		}

		_gui.setInputImage( _input->frame() );


	}

	inline void MIApp::selectionDidStart()
	{
		_selectingTemplate = true;
	}


	inline void MIApp::selectionFinished( const Rectf& r )
	{
		Recti roi( r.x * _currGray.width(),
				   r.y * _currGray.height(),
				   r.width * _currGray.width(),
				   r.height * _currGray.height() );

		if( roi.width < 10 || roi.height < 10 )
			return;

		Image patch( _currGray, &roi );

		_tracker.updateTemplate( patch );
		_gui.setTemplateImage( patch, 
							   _tracker.templateGradX(), 
							   _tracker.templateGradY() );
		_selectingTemplate = false;
	}
}
