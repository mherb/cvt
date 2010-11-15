#ifndef CVT_APPLICATION_H
#define CVT_APPLICATION_H

#include <stdlib.h>
#include <stdint.h>

#include <cvt/gui/EventTimer.h>

namespace cvt {
	class WidgetImpl;
	class Widget;

	class Application {
		friend class Widget;

		public:
			static void run() { instance()->runApp(); };
			static void exit() { instance()->exitApp(); };

			virtual uint32_t registerTimer( size_t interval, EventTimer* t ) = 0;
			virtual void unregisterTimer( uint32_t id ) = 0;

			static Application* instance();
		protected:
			Application() {};
			Application( const Application& );
			virtual ~Application() {};

			virtual WidgetImpl* getWidgetImpl( Widget* widget ) = 0;

			virtual void runApp() = 0;
			virtual void exitApp() = 0;

			static Application* app;
	};
}

#endif
