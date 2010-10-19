#ifndef CVTAPPLICATION_H
#define CVTAPPLICATION_H

namespace cvt {
	class WidgetImpl;
	class Widget;

	class Application {
		friend class Widget;

		public:
			static void run() { instance()->runApp(); };
			static void exit() { instance()->exitApp(); };

		protected:
			Application() {};
			Application( const Application& );
			virtual ~Application() {};

			virtual WidgetImpl* getWidgetImpl( Widget* widget ) = 0;
			virtual void runApp() = 0;
			virtual void exitApp() = 0;

			static Application* instance();
			static Application* app;
	};
}

#endif
