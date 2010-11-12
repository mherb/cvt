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
			EventThreadX11( ::Display* display, TQueue<std::pair< ::Window,Event*> >* q ) : dpy( display ), _queue( q ) {};
			~EventThreadX11() {};
			void execute( void* );

		private:
			inline void enqueue( ::Window w, Event* e ) { _queue->enqueue( std::make_pair< ::Window, Event*>( w, e ) ); }

			::Display* dpy;
			TQueue<std::pair< ::Window,Event*> >* _queue;
			::Atom xatom_wmdelete;
			::Atom xatom_wmproto;
	};
};


#endif
