#ifndef CVT_EVENTTHREADX11_H
#define CVT_EVENTTHREADX11_H

#include <cvt/gui/internal/X.h>
#include <cvt/gui/internal/WidgetImplWinGLX11.h>
#include <cvt/util/Thread.h>
#include <cvt/util/TQueue.h>
#include <cvt/gui/event/Event.h>
#include <map>

namespace cvt {

	class EventThreadX11 : public Thread<void>  {
		public:
			EventThreadX11( ::Display* display, TQueue<std::pair<WidgetImplWinGLX11*,Event*> >* q, std::map< ::Window, WidgetImplWinGLX11*>& winmap ) : dpy( display ), _queue( q ), windows( winmap ) {};
			~EventThreadX11() {};
			void execute( void* );

		private:
			inline void enqueue( WidgetImplWinGLX11* w, Event* e ) { _queue->enqueue( std::make_pair<WidgetImplWinGLX11*, Event*>( w, e ) ); }

			::Display* dpy;
			TQueue<std::pair<WidgetImplWinGLX11*,Event*> >* _queue;
			std::map< ::Window, WidgetImplWinGLX11*> windows;
			::Atom xatom_wmdelete;
			::Atom xatom_wmproto;
	};
};


#endif
