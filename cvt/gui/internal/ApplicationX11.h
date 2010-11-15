#ifndef CVTAPPLICATIONX11_H
#define CVTAPPLICATIONX11_H

#include <cvt/gui/Application.h>
#include <cvt/gui/internal/X.h>
#include <cvt/gui/internal/WidgetImplWinGLX11.h>
#include <cvt/gui/event/Event.h>
#include <cvt/gui/EventTimer.h>
#include <cvt/gui/TimerInfoList.h>
#include <cvt/gl/OpenGL.h>
#include <cvt/util/TQueue.h>
#include <map>
#include <queue>
#include <utility>

namespace cvt {
	class ApplicationX11 : public Application
	{
		public:
			ApplicationX11();
			~ApplicationX11();

			virtual void runApp();
			virtual void exitApp() { run = false; };

			virtual uint32_t registerTimer( size_t interval, EventTimer* t ) { return timers.registerTimer( interval, t ); };
			virtual void unregisterTimer( uint32_t id ) { timers.unregisterTimer( id ); };

			virtual WidgetImpl* getWidgetImpl( Widget* win );

		private:
			ApplicationX11( const Application& );

			::Display* dpy;
			::GLXContext ctx;
			::XVisualInfo *visinfo;
			::Atom xatom_wmdelete;
			bool run;
			std::map< ::Window, WidgetImplWinGLX11*> windows;
			TQueue<std::pair< ::Window,Event*> > events;
			TimerInfoList timers;
	};
}

#endif
