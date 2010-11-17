#ifndef CVTAPPLICATIONX11_H
#define CVTAPPLICATIONX11_H

#include <cvt/gui/Application.h>
#include <cvt/gui/internal/X.h>
#include <cvt/gui/internal/WidgetImplWinGLX11.h>
#include <cvt/gui/event/Event.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/internal/TimerInfoList.h>
#include <cvt/gl/OpenGL.h>
#include <cvt/io/IOSelect.h>
#include <map>
#include <deque>

namespace cvt {
	class ApplicationX11 : public Application
	{
		public:
			ApplicationX11();
			~ApplicationX11();

			virtual void runApp();
			virtual void exitApp() { run = false; };

			virtual uint32_t _registerTimer( size_t interval, TimeoutHandler* t ) { return _timers.registerTimer( interval, t ); };
			virtual void _unregisterTimer( uint32_t id ) { _timers.unregisterTimer( id ); };

			virtual WidgetImpl* getWidgetImpl( Widget* win );

		private:
			ApplicationX11( const Application& );

			::Display* dpy;
			::GLXContext ctx;
			::XVisualInfo *visinfo;
			::Atom xatom_wmdelete;
			bool run;
			std::map< ::Window, WidgetImplWinGLX11*> windows;
			std::deque< WidgetImplWinGLX11*> updates;
			IOSelect _ioselect;
			TimerInfoList _timers;
	};
}

#endif
