#ifndef CVT_WIDGETCONTAINER_H
#define CVT_WIDGETCONTAINER_H

#include <cvt/gui/Widget.h>
#include <cvt/gui/WidgetLayout.h>
#include <list>
#include <utility>

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
			size_t	childrenCount() const;
			Widget* childAt( int x, int y );

		protected:
			void paintChildren( GFX* gfx, const Recti& r );
			void resizeEvent( ResizeEvent* event );
			void paintEvent( PaintEvent* event, GFX* gfx );

		private:
			typedef std::list< std::pair<Widget*, WidgetLayout> > ChildList;

			WidgetContainer( bool toplevel );

			//	    std::list<Widget*> _mchildren;
			ChildList _children;
	};

	inline size_t WidgetContainer::childrenCount() const
	{
		return _children.size();
	}


	inline void WidgetContainer::addWidget( Widget* w, const WidgetLayout& layout )
	{
		w->setParent( this );
		_children.push_back( std::make_pair<Widget*, WidgetLayout>( w, layout ) );
	}

}

#endif
