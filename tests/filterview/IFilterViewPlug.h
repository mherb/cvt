#ifndef CVT_IFILTERVIEW_PLUG_H
#define CVT_IFILTERVIEW_PLUG_H

#include <cvt/gui/Widget.h>
#include <cvt/math/Vector.h>

namespace cvt {
	class IFilterView;

	class IFilterViewPlug : public Widget {
		public:
			IFilterViewPlug( bool input, ParamType type, size_t handle );
			~IFilterViewPlug();

			IFilterView* view() const;
			bool isConnected() const;
			void setConnected( bool b );
			Vector2i center() const;
			bool isInput() const;
			ParamType type() const;

			void paintEvent( PaintEvent* , GFX* g );
		private:
			bool _isconnected;
			bool _isinput;
			ParamType _type;
			size_t _handle;
	};

	inline IFilterViewPlug::IFilterViewPlug( bool input, ParamType ptype, size_t handle ) : _isconnected( false ), _isinput( input ), _type( ptype ), _handle( handle )
	{
	}

	inline IFilterViewPlug::~IFilterViewPlug()
	{
	}

	inline bool IFilterViewPlug::isConnected() const
	{
		return _isconnected;
	}


	inline bool IFilterViewPlug::isInput() const
	{
		return _isinput;
	}

	inline ParamType IFilterViewPlug::type() const
	{
		return _type;
	}

	inline void IFilterViewPlug::setConnected( bool b )
	{
		_isconnected = b;
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
		if( _isconnected ) {
			g->color().set( 0.8f, 0.9f, 0.1f, 1.0f );
			g->drawIcon( w / 2 - 8, h / 2 - 8, GFX::ICON_CIRCLE );
		} else {
			g->color().set( 0.8f, 0.8f, 0.8f, 1.0f );
			g->drawIcon( w / 2 - 8, h / 2 - 8, GFX::ICON_CIRCLEEMPTY );
		}
	}

}

#endif
