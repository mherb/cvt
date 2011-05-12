#ifndef CVT_IFILTERVIEW_PLUG_H
#define CVT_IFILTERVIEW_PLUG_H

#include <cvt/gui/Widget.h>
#include <cvt/math/Vector.h>

namespace cvt {
	class IFilterView;

	class IFilterViewPlug : public Widget {
		public:
			enum State { CONNECTED, UNCONNECTED, CONNECTABLE, UNCONNECTABLE };

			IFilterViewPlug( bool input );
			~IFilterViewPlug();

			IFilterView* view() const;
			State state() const;
			void setState( State s );
			Vector2i center() const;
			bool isInput() const;

			void paintEvent( PaintEvent* , GFX* g );

		private:
			bool _isinput;
			State _state;
	};

	inline IFilterViewPlug::IFilterViewPlug( bool input ) : _isinput( input ), _state( UNCONNECTED )
	{
	}

	inline IFilterViewPlug::~IFilterViewPlug()
	{
	}

	inline IFilterViewPlug::State IFilterViewPlug::state() const
	{
		return _state;
	}


	inline bool IFilterViewPlug::isInput() const
	{
		return _isinput;
	}

	inline void IFilterViewPlug::setState( State s )
	{
		_state = s;
		update();
	}

	inline Vector2i IFilterViewPlug::center() const
	{
		Vector2i ret;
		size( ret.x, ret.y );
		ret.x /= 2;
		ret.y /= 2;
		mapGlobal( ret.x, ret.y );
		return ret;
	}

	inline IFilterView* IFilterViewPlug::view() const
	{
		return ( IFilterView* ) parent();
	}

	inline void IFilterViewPlug::paintEvent( PaintEvent* , GFX* g )
	{
		int w, h;
		size( w, h );
		if( _state == CONNECTED ) {
			g->color().set( 0.8f, 0.9f, 0.1f, 1.0f );
			g->drawIcon( w / 2 - 8, h / 2 - 8, GFX::ICON_CIRCLE );
		} if( _state == CONNECTABLE ) {
			g->color().set( 0.8f, 0.9f, 0.1f, 1.0f );
			g->drawIcon( w / 2 - 8, h / 2 - 8, GFX::ICON_CIRCLEEMPTY );
		} else if( _state == UNCONNECTED ){
			g->color().set( 0.8f, 0.8f, 0.8f, 1.0f );
			g->drawIcon( w / 2 - 8, h / 2 - 8, GFX::ICON_CIRCLEEMPTY );
		} else if( _state == UNCONNECTABLE ) {
			g->color().set( 1.0f, 0.0f, 0.0f, 1.0f );
			g->drawIcon( w / 2 - 8, h / 2 - 8, GFX::ICON_CIRCLEEMPTY );
		}
	}

}

#endif
