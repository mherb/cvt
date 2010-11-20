#ifndef CVT_WIDGETCONTAINER_H
#define CVT_WIDGETCONTAINER_H

#include <cvt/gui/Widget.h>
#include <cvt/gui/WidgetLayout.h>
#include <list>

namespace cvt {

	class WidgetContainer : public Widget
	{
		friend class Window;

		public:
			WidgetContainer();
			~WidgetContainer();

			void    addWidget( Widget* w, const WidgetLayout& layout );
			//	    void    addWidgetMoveable( Widget* w );
			void    removeWidget( Widget* w );
			Widget* childAt( int x, int y );

		protected:
			void paintChildren( GFX* gfx );

		private:
			WidgetContainer( bool toplevel );

			//	    std::list<Widget*> _mchildren;
			std::list<Widget*> _children;
	};
}

#endif
