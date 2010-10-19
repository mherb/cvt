#ifndef CVTAPPLICATIONX11_H
#define CVTAPPLICATIONX11_H

#include <cvt/gui/Application.h>
#include <cvt/gui/internal/X.h>
#include <cvt/gui/internal/WidgetImplWinGLX11.h>
#include <cvt/gui/event/Event.h>
#include <map>

namespace cvt {
	class ApplicationX11 : public Application
	{
		public:
			ApplicationX11();
			~ApplicationX11();

			virtual WidgetImpl* getWidgetImpl( Widget* win );
			virtual void runApp();
			virtual void exitApp() { run = false; };

		private:
			ApplicationX11( const Application& );

			::Display* dpy;
			::GLXContext ctx;
			::XVisualInfo *visinfo;
			bool run;
			std::map< ::Window, WidgetImplWinGLX11*> windows;
			::Atom xatom_wmdelete;
			::Atom xatom_wmproto;

	};
}

#endif
