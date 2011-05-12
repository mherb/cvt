#ifndef CVT_IFILTERVIEW_H
#define CVT_IFILTERVIEW_H

#include <cvt/gui/WidgetContainer.h>
#include <cvt/gfx/IFilter.h>
#include <cvt/gui/Label.h>
#include <cvt/util/String.h>

#include "IFilterViewPlug.h"

namespace cvt {
	class IFilterView : public WidgetContainer {
		public:
			IFilterView( IFilter* filter );
			~IFilterView();

			const String& name() const { return _name; }
			void paintEvent( PaintEvent* , GFX* gfx );
		private:
			void parameterToWidget( const ParamInfo* pinfo );

			IFilter*  _filter;
			ParamSet* _pset;
			String	  _name;
			std::list<Widget*> _pwidgets;
	};

	inline IFilterView::IFilterView( IFilter* filter ) : _filter( filter ), _name( filter->name().c_str() )
	{
		_pset = filter->parameterSet();

		for( size_t i = 0; i < _pset->size(); i++ ) {
			parameterToWidget( _pset->paramInfo( i ) );
		}
		//FIXME: use label minsize resp. join of child minsize
		setMinimumSize( 100, ( _pwidgets.size() / 2 ) * 20 );
	}

	inline IFilterView::~IFilterView()
	{
		for( std::list<Widget*>::const_iterator it = _pwidgets.begin(), end = _pwidgets.end(); it != end; ++it )
			delete *it;
		_pwidgets.clear();
	}

	inline void IFilterView::parameterToWidget( const ParamInfo* pinfo )
	{
		Label* label = new Label( pinfo->name.c_str(), pinfo->isInput?( ALIGN_LEFT | ALIGN_VCENTER ) : ( ALIGN_RIGHT | ALIGN_VCENTER ) );
		label->color().set( 0.0f, 0.0f, 0.0f, 1.0f );
		size_t num = _pwidgets.size() / 2;
		_pwidgets.push_back( label );
		WidgetLayout wl;
		wl.setAnchoredLeftRight( 20, 20 );
		wl.setAnchoredTop( num * 20, 20 );
		addWidget( label, wl );
		IFilterViewPlug* plug = new IFilterViewPlug( pinfo->isInput );
		_pwidgets.push_back( plug );
		if( pinfo->isInput )
			wl.setAnchoredLeft( 0, 20 );
		else
			wl.setAnchoredRight( 0, 20 );
		wl.setAnchoredTop( num * 20, 20 );
		addWidget( plug, wl );
	}

	inline void IFilterView::paintEvent( PaintEvent* , GFX* gfx )
	{
		int w, h;

		size( w, h );
		gfx->color().set( 0.6f, 0.6f, 0.6f, 0.8f );
		gfx->fillRoundRect( 0, 0, w, h, 5.0f );

		Recti r = rect();
		paintChildren( gfx, r );
	}
}

#endif
