#ifndef CVT_APPLICATIONXOSX_H
#define CVT_APPLICATIONXOSX_H

#include <cvt/gui/Application.h>
#include <cvt/gui/event/Event.h>
#include <cvt/gl/OpenGL.h>
#include <cvt/gui/internal/OSX/CGLContext.h>
#include <cvt/gui/internal/OSX/TimerInfoListOSX.h>
#include <map>
#include <deque>

namespace cvt {
	struct OSXData;
	class ApplicationOSX : public Application
	{
		public:
			ApplicationOSX();
			~ApplicationOSX();

			virtual void runApp();
			virtual void exitApp();

			virtual uint32_t _registerTimer( size_t interval, TimeoutHandler* t ) { return _timers.registerTimer( interval, t ); };
			virtual void _unregisterTimer( uint32_t id ) { _timers.unregisterTimer( id ); };


		private:
			ApplicationOSX( const Application& );

			virtual WidgetImpl* _registerWindow( Widget* w );
			virtual void _unregisterWindow( WidgetImpl* w );

			virtual bool _hasGLSupport() { return true; }
			virtual bool _hasCLSupport() { return _clsupport; }

			virtual GLContext* _defaultGLContext() { return _defaultctx; }

			CGLContext* _defaultctx;
			bool _clsupport;
			TimerInfoListOSX _timers;
			OSXData* _osx;
	};
}

#endif
