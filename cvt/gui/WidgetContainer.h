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
			void    addWidget( Widget* w );
			void    removeWidget( Widget* w );
			size_t	childrenCount() const;
			Widget* childAt( int x, int y );

			void resizeEvent( ResizeEvent* event );
			void paintEvent( PaintEvent* event, GFX* gfx );
			void mousePressEvent( MousePressEvent* event );
			void mouseMoveEvent( MouseMoveEvent* event );
			void mouseReleaseEvent( MouseReleaseEvent* event );

		protected:
			void paintChildren( GFX* gfx, const Recti& r );
			void resizeChildren( );

		private:
			typedef std::list< std::pair<Widget*, WidgetLayout> > ChildList;

			WidgetContainer( bool toplevel );

			ChildList _children;
			Widget* _activeWidget;
	};

	inline size_t WidgetContainer::childrenCount() const
	{
		return _children.size();
	}


	inline void WidgetContainer::addWidget( Widget* w, const WidgetLayout& layout )
	{
		w->setParent( this );
		w->setVisible( true );
		_children.push_back( std::make_pair<Widget*, WidgetLayout>( w, layout ) );
	}

	inline void WidgetContainer::addWidget( Widget* w )
	{
		w->setParent( this );
		w->setVisible( true );
		_children.push_back( std::make_pair<Widget*, WidgetLayout>( w, WidgetLayout() ) );
	}


}

#endif
