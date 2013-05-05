/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_WIDGETCONTAINER_H
#define CVT_WIDGETCONTAINER_H

#include <cvt/gui/Widget.h>
#include <cvt/gui/WidgetLayout.h>
#include <list>
#include <utility>

namespace cvt {
	class Window;

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

			void resizeEvent( ResizeEvent& event );
			void moveEvent( MoveEvent& event );
			void paintEvent( PaintEvent& event, GFX& gfx );
			void mousePressEvent( MousePressEvent& event );
			void mouseMoveEvent( MouseMoveEvent& event );
			void mouseReleaseEvent( MouseReleaseEvent& event );
			void keyPressEvent( KeyEvent& event );
			void keyReleaseEvent( KeyEvent& event );

		protected:
			void paintChildren( GFX& gfx, const Recti& r );
			void resizeChildren( );
			void moveChildren( int dx, int dy );
			void raiseChild( Widget* c );
			void lowerChild( Widget* c );

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
		_children.push_back( std::pair<Widget*, WidgetLayout>( w, layout ) );
		resizeChildren();
	}

	inline void WidgetContainer::addWidget( Widget* w )
	{
		w->setParent( this );
		w->setVisible( true );
		_children.push_front( std::pair<Widget*, WidgetLayout>( w, WidgetLayout() ) );
		resizeChildren();
	}


}

#endif
