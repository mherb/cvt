/*
 * File:   VideoInputGui.h
 * Author: sebi
 *
 * Created on July 15, 2011, 9:22 AM
 */

#ifndef VIDEOINPUTGUI_H
#define	VIDEOINPUTGUI_H

#include <cvt/util/Time.h>

#include <cvt/gui/Window.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Moveable.h>

#include <cvt/gfx/Image.h>
#include <cvt/io/VideoInput.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/math/sac/RANSAC.h>
#include <cvt/math/sac/HomographySAC.h>
#include <cvt/vision/FeatureMatch.h>

#include "ORBTemplateTracker.h"
#include "ImageViewHomography.h"

namespace cvt
{

class VideoInputGui : public Window, TimeoutHandler
{
	public:
		VideoInputGui( VideoInput * cam, const cvt::Image & reference ) :
            Window( "Camera Application" ),
            _cam( cam ),
            _moveable( &_imView ),
            _templateMov( &_templateView ),
            _quitButton( "Quit" ),
            _orbTracker( reference )
		{
            initGuiElements();

			_refWidth = reference.width();
			_refHeight = reference.height();

            _timerId = Application::registerTimer( 20, this );
            _timer.reset();
			_processingTime.reset();

			_templateView.setImage( reference );
		}

        ~VideoInputGui()
		{
            Application::unregisterTimer( _timerId );
		}

  private:
        void initGuiElements()
        {
            setSize( 800, 600 );

            _moveable.setSize( 320, 240 );
            addWidget( &_moveable );
            
			_templateMov.setSize( 320, 240 );
            addWidget( &_templateMov );

            _quitButton.clicked.add( Delegate<void ()>( &Application::exit ) );

            WidgetLayout wl;
            wl.setAnchoredRight( 10, 100 );
            wl.setAnchoredBottom( 10, 20 );
            addWidget( &_quitButton, wl );
            wl.setAnchoredBottom( 40, 20 );

            _moveable.setTitle( "ORB Estimate" );
            _templateMov.setTitle( "Template Image" );

            setVisible( true );
        }

		void onTimeout()
		{
            _cam->nextFrame();

            Image gray;
			_cam->frame().convert( gray, IFormat::GRAY_UINT8 );

			_imView.setImage( gray );

            Matrix3f h;
            if( _orbTracker.estimate( h, gray ) ){
                std::cout << h << std::endl;
				Vector2f pt0 = h * Vector2f( 0.0f, 0.0f );
				pt0.x /= _cam->width();
				pt0.y /= _cam->height();
				Vector2f pt1 = h * Vector2f( _refWidth, 0.0f );
				pt1.x /= _cam->width();
				pt1.y /= _cam->height();
				Vector2f pt2 = h * Vector2f( _refWidth, _refHeight );
				pt2.x /= _cam->width();
				pt2.y /= _cam->height();
				Vector2f pt3 = h * Vector2f( 0.0f, _refHeight );
				pt3.x /= _cam->width();
				pt3.y /= _cam->height();
				_imView.setPoints( pt0, pt1, pt2, pt3 );
            }

			_processingTime.reset();
        }

	private:
		VideoInput*  _cam;
        ImageViewHomography   _imView;
        Moveable	 _moveable;
        ImageView    _templateView;
        Moveable	 _templateMov;
        Button       _quitButton;

		Time		_timer;
		Time		_processingTime;

        ORBTemplateTracker  _orbTracker;
		float		_refWidth;
		float		_refHeight;

        uint32_t    _timerId;

};

}



#endif	/* VIDEOINPUTGUI_H */

