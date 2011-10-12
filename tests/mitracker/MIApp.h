#include <cvt/gui/Window.h>
#include <cvt/gui/Application.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/util/Time.h>

#include "TemplateDetector.h"

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
			VideoInput*			_input;
			MIGui				_gui;
			MITracker			_tracker;
			TemplateDetector	_detector;
			uint32_t			_timerId;
			Time				_fpsTime;
			size_t				_iter;
			bool				_selectingTemplate;
			Image				_currGray;

			void calculateRectPoints( std::vector<Vector2f>& pts );
	};

	inline MIApp::MIApp( VideoInput* input ) :
		_input( input ),
		_iter( 0 ),
		_selectingTemplate( true )
	{
		_timerId = Application::registerTimer( 20, this );

		// add the delegates to the gui
		Delegate<void (void)> selStart( this, &MIApp::selectionDidStart );
		Delegate<void (const Rectf&)> selDone( this, &MIApp::selectionFinished );
		Delegate<void (size_t)> maxIterChanged( &_tracker, &MITracker::setMaxIterations );

		_gui.addSelectionDelegates( selStart, selDone );
		_gui.observeMaxIterations( maxIterChanged );
		_gui.setMaxIter( _tracker.maxIterations() );
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
			Image diff( _tracker.templateImage() );
			diff.sub( _tracker.warped() );
			_gui.setTemplateImage( _tracker.templateImage(), 
								  _tracker.warped(), 
								  diff );
			std::vector<Vector2f> pts;
			calculateRectPoints( pts );
			_gui.setPoints( pts );
		}

		if( _fpsTime.elapsedSeconds() > 2 ){
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

		Vector2f pos;
		pos.x = roi.x; pos.y = roi.y;		
		_tracker.updateTemplate( patch, pos );
		_gui.setTemplateImage( patch, 
							   _tracker.templateGradX(), 
							   _tracker.templateGradY() );
		_selectingTemplate = false;
	}

	inline void MIApp::calculateRectPoints( std::vector<Vector2f>& pts )
	{
		const Matrix3f& h = _tracker.pose();
		const Image & temp = _tracker.templateImage();

		Vector2f p;
		pts.resize( 4 );
		p.x = 0; p.y = 0;
		pts[ 0 ] = h * p;
		p.x = temp.width(); p.y = 0;
		pts[ 1 ] = h * p;
		p.x = temp.width(); p.y = temp.height();
		pts[ 2 ] = h * p;
		p.x = 0; p.y = temp.height();
		pts[ 3 ] = h * p;

		for( size_t i = 0; i < 4; i++ ){
			pts[ i ].x /= (float)_currGray.width();
			pts[ i ].y /= (float)_currGray.height();
		}
	}
}
