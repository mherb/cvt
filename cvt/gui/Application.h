#ifndef CVT_APPLICATION_H
#define CVT_APPLICATION_H

#include <stdlib.h>
#include <stdint.h>

#include <cvt/gui/TimeoutHandler.h>

namespace cvt {
	class WidgetImpl;
	class Widget;

	class Application {
		friend class Widget;

		public:
			static void run() { instance()->runApp(); };
			static void exit() { instance()->exitApp(); };

			static uint32_t registerTimer( size_t interval, TimeoutHandler* t ) { return instance()->_registerTimer( interval, t ); }
			static void unregisterTimer( uint32_t id ) { instance()->_unregisterTimer( id ); }

		protected:
			Application() {};
			Application( const Application& );
			virtual ~Application() {};

			virtual WidgetImpl* getWidgetImpl( Widget* widget ) = 0;
			virtual uint32_t _registerTimer( size_t interval, TimeoutHandler* t ) = 0;
			virtual void _unregisterTimer( uint32_t id ) = 0;


			virtual void runApp() = 0;
			virtual void exitApp() = 0;

			static Application* instance();
			static Application* app;
	};
}

#endif
