#include <cvt/gui/Window.h>
#include <cvt/gui/Application.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/util/Time.h>

#include "TemplateDetector.h"

//#include "MITracker.h"
#include "MITrackerApprox.h"

#include "MIGui.h"

//#define DEBUGMI 1

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

			enum State
			{
				SELECTING = 0,
				TRACKING,
				DETECTING
			};

			State				_state;

			VideoInput*			_input;
			MIGui				_gui;
			MITracker			_tracker;
			TemplateDetector	_detector;
			uint32_t			_timerId;
			Time				_fpsTime;
			size_t				_iter;
			Image				_currGray;

			void calculateRectPoints( std::vector<Vector2f>& pts );
	};

	inline MIApp::MIApp( VideoInput* input ) :
		_state( SELECTING ),
		_input( input ),
		_iter( 0 )
	{
		_timerId = Application::registerTimer( 20, this );

		// add the delegates to the gui
		Delegate<void (void)> selStart( this, &MIApp::selectionDidStart );
		Delegate<void (const Rectf&)> selDone( this, &MIApp::selectionFinished );
		Delegate<void (size_t)> maxIterChanged( &_tracker, &MITracker::setMaxIterations );

		_gui.addSelectionDelegates( selStart, selDone );
		_gui.observeMaxIterations( maxIterChanged );
		_gui.setMaxIter( _tracker.maxIterations() );
#ifdef DEBUGMI
		{
			Image tmp;
			tmp.load("../data/lena_g.png");
			tmp.convert( _currGray, IFormat::GRAY_FLOAT );

			_tracker.updateTemplate( _currGray, Vector2f( 0, 0 ) );
			_state = TRACKING;
			tmp.reallocate( 800, 800, IFormat::GRAY_FLOAT );
			tmp.fill( Color::WHITE );
			tmp.copyRect( ( 800 - 512 ) / 2, ( 800 - 512 ) / 2, _currGray, Recti( 0, 0, 512, 512 ) );
			_currGray.reallocate( tmp );
			_currGray = tmp;
			_tracker.setMaxIterations( 1 );
			_gui.setMaxIter( _tracker.maxIterations() );
		}
#endif
	}

	inline MIApp::~MIApp()
	{
		Application::unregisterTimer( _timerId );
	}

	inline void MIApp::onTimeout()
	{
		_iter++;
#ifdef DEBUGMI
		static float tx = -10.0f;
		static float ty = -10.0f;
#else
		_input->nextFrame();
		_input->frame().convert( _currGray, IFormat::GRAY_UINT8 );
#endif

		switch( _state ){
			case SELECTING: 
				break;
			case TRACKING:
				{	
#ifdef DEBUGMI
					Matrix3f hom;
					hom.setHomography( 0.0f, 0.0f, 1.0f, 1.0f, 144.0f + tx , 144.0f + ty, 0.0f, 0.0f );
					_tracker.setPose( hom );
#endif

					_tracker.updateInput( _currGray );

#ifdef DEBUGMI
					std::cout << tx << " " << ty
							  << " " << _tracker.MI()
							  << " " << _tracker.jacobian()( 0 )
							  << " " << _tracker.jacobian()( 1 )
							  << " " << _tracker.hessian()( 0, 0 )
							  << " " << _tracker.hessian()( 0, 1 )
							  << " " << _tracker.hessian()( 1, 0 )
							  << " " << _tracker.hessian()( 1, 1 )
							  << std::endl;
					tx += 0.5f;
					if( tx >= 10.025f ) {
						std::cout << std::endl;
						tx = -10.0f;
						ty += 0.5f;
						if( ty >= 10.025f )
							exit( 1 );
					}
#endif

					/*if( !_detector.checkHomography( _tracker.pose() ) ){
						_state = DETECTING;
					}*/

					Image diff( _tracker.templateImage() );
					diff.sub( _tracker.warped() );
					_gui.setTemplateImage( _tracker.templateImage(), 
                                           _tracker.warped(),
                                           diff );
					std::vector<Vector2f> pts;
					calculateRectPoints( pts );
					_gui.setPoints( pts );
				}
				break;
			case DETECTING:
				{
					Matrix3f detectedHom;
					if( _detector.detect( detectedHom, _currGray ) ){
						_state = TRACKING;
						_tracker.setPose( detectedHom );

						std::vector<Vector2f> pts;
						calculateRectPoints( pts );
						_gui.setPoints( pts );
					}
				}
			default:
				break;
		}

		if( _fpsTime.elapsedSeconds() > 2 ){
			_gui.setFPS( _iter * 1000.0f / _fpsTime.elapsedMilliSeconds() );
			_fpsTime.reset();
			_iter = 0;
		}

		_gui.setInputImage( _currGray );
	}

	inline void MIApp::selectionDidStart()
	{
		_state = SELECTING;	
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
		_state = TRACKING;
	//	_detector.updateTemplate( patch );
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
