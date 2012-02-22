/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_X11HANDLER_H
#define CVT_X11HANDLER_H

#include <cvt/gui/internal/X11/X.h>
#include <cvt/gui/internal/X11/WidgetImplWinGLX11.h>

#include <cvt/io/IOHandler.h>

#include <map>

namespace cvt {
	class X11Handler : public IOHandler
	{
		public:
			X11Handler( ::Display* dpy, std::map< ::Window, WidgetImplWinGLX11*>* windows );
			~X11Handler();
			virtual void onDataReadable();
			void handleQueued();

		private:
			X11Handler( const X11Handler& );
			void handleEvents();

			::Display* _dpy;
			std::map< ::Window, WidgetImplWinGLX11*>* _windows;
			::Atom xatom_wmdelete;
			::Atom xatom_wmproto;
	};

	inline X11Handler::X11Handler( ::Display* dpy, std::map< ::Window, WidgetImplWinGLX11*>* windows ) : IOHandler(), _dpy( dpy ), _windows( windows )
	{
		_fd = ConnectionNumber( _dpy );
		notifyReadable( true );
		xatom_wmproto = XInternAtom( _dpy , "WM_PROTOCOLS", False);
		xatom_wmdelete = XInternAtom( _dpy, "WM_DELETE_WINDOW", False);
	}

	inline X11Handler::~X11Handler()
	{
	}
}

#endif
