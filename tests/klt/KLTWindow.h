#ifndef CVT_KLT_WINDOW_H
#define CVT_KLT_WINDOW_H

#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>

#include <cvt/vision/FAST.h>
#include <cvt/io/RawVideoReader.h>
#include <cvt/io/Resources.h>
#include <cvt/util/Time.h>

#include "KLTTracker.h"

#include <cvt/gfx/GFXEngineImage.h>

namespace cvt {

	class KLTWindow : public TimeoutHandler
	{
		public:
			KLTWindow( VideoInput & video ) :
				_window( "KLT" ),	
				_video( video ),
				_klt( 20, 32 )
			{
				_timerId = Application::registerTimer( 10, this );
				_window.setSize( 320, 240 );
				WidgetLayout wl;
				wl.setAnchoredTopBottom( 0, 0 );
				wl.setAnchoredLeftRight( 0, 0 );
				_window.addWidget( &_imView, wl );
				_window.setVisible( true );
				_window.update();				

				_video.frame().convert( _lastFrame, IFormat::GRAY_UINT8 );
				
				/*
				Resources r;
				_lena.load( r.find( "lena_g.png" ) );
				_currR.x = _lena.width() / 2.0f - 50;
				_currR.y = _lena.height() / 2.0f - 50;
				_currR.width = _lena.width() / 2.0f;
				_currR.height = _lena.height() / 2.0f;

				_offX = 1;
				_offY = 1;

				_lastFrame.reallocate( _currR.width, _currR.height, IFormat::GRAY_UINT8 );
				_lastFrame.copyRect( 0, 0, _lena, _currR );
*/
			
				VectorVector2Inserter<float> inserter( _lastFeatures );
				FAST::detect9( _lastFrame, 40, inserter, 20 );

				_imView.setImage( _lastFrame );
			}

			~KLTWindow()
			{
				Application::unregisterTimer( _timerId );
			}

			void onTimeout();

		private:
			Image					_lena;
			Recti					_currR;
			int						_offX;
			int						_offY;
			uint32_t				_timerId;
			Window					_window;
			ImageView				_imView;
			VideoInput &			_video;
			Image				    _lastFrame;
			std::vector<Vector2f>	_lastFeatures;

			KLTTracker	_klt;
			Time		_time;

			void drawFeatures( Image & img, const std::vector<Vector2f> & positions, const Color & c );

	};

	inline void KLTWindow::onTimeout()
	{
		Image frame;
		_video.frame().convert( frame, IFormat::GRAY_UINT8 );
				
		//frame.reallocate( _currR.width, _currR.height, IFormat::GRAY_UINT8 );
		//frame.copyRect( 0, 0, _lena, _currR );

		std::vector<Vector2f> trackedPositions;
		std::vector<size_t>	  trackedIndices;
		_klt.trackFeatures( trackedIndices, trackedPositions, frame, _lastFrame, _lastFeatures );

		Image debug;
		frame.convert( debug, IFormat::RGBA_UINT8 );
		drawFeatures( debug, _lastFeatures, Color::RED );
		drawFeatures( debug, trackedPositions, Color::GREEN );
		_imView.setImage( debug );

		double fps = 1.0 / _time.elapsedSeconds();
		_time.reset();

		String title;
		title.sprintf( "KLT: tracked=%d, all=%d, fps=%0.2f", _lastFeatures.size(), trackedPositions.size(), fps );
		std::cout << title << std::endl;
		_window.setTitle( title );

		_lastFrame = frame;
		_lastFeatures = trackedPositions;

		//_currR.x += _offX;
		//_currR.y += _offY;
		//if( ( _currR.x + _currR.width ) > (int)( _lena.width() - 50 ) || _currR.x < 50 )
		//	_offX *= -1;
		//if( ( _currR.y + _currR.height ) > (int)( _lena.height() - 50 ) || _currR.y < 50 )
		//	_offY *= -1;


		_video.nextFrame();
	}
	
	inline void KLTWindow::drawFeatures( Image & img, const std::vector<Vector2f> & positions, const Color & c )
	{
		{
			GFXEngineImage ge( img );
			GFX g( &ge );
			g.color() = c;

			Recti r;
			size_t pSize = 31;
			size_t phSize = pSize >> 1;
			r.width = pSize;
			r.height = pSize;
			for( size_t i = 0; i < positions.size(); i++ ){
				r.x = ( int )positions[ i ].x - phSize;
				r.y = ( int )positions[ i ].y - phSize;
				g.drawRect( r );
			}
		}
	}
}

#endif
