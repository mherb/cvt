#ifndef CVT_APPLICATIONXOSX_H
#define CVT_APPLICATIONXOSX_H

#include <cvt/gui/Application.h>
#include <cvt/gui/event/Event.h>
#include <cvt/gl/OpenGL.h>
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

			virtual uint32_t _registerTimer( size_t interval, TimeoutHandler* t );
			virtual void _unregisterTimer( uint32_t id );


		private:
			ApplicationOSX( const Application& );

			virtual WidgetImpl* _registerWindow( Widget* w );
			virtual void _unregisterWindow( WidgetImpl* w );

			virtual bool _hasGLSupport() { return true; }
			virtual bool _hasCLSupport() { return _clsupport; }

			bool _clsupport;
			OSXData* _osx;
	};
}

#endif
