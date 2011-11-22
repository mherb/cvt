#ifndef CVT_PLOTDATA_H
#define CVT_PLOTDATA_H

#include <cvt/util/Signal.h>
#include <cvt/math/Vector.h>

namespace cvt {
	enum PlotStyle {
		PLOT_STYLE_DOTS,
		PLOT_STYLE_LINES
	};

	class PlotData {
		public:
			PlotData();
			~PlotData();

			PlotStyle plotStyle() const;
			void	  setPlotStyle( PlotStyle style );
			void	  setPlotColor( const Color& c );
			const	  Color& plotColor() const;
			void	  setPlotSize( float s );
			float	  plotSize() const;
			void      setName( const String& title );
			const     String& name() const;

			//Point2f*  data(  )

			Signal<PlotData*> changed;

		private:
			PlotStyle _style;
			Color	  _color;
			float	  _size;
			String    _name;
	};

	inline PlotData::PlotData() :
		_style( PLOT_STYLE_LINES ),
		_color( Color::RED ),
		_size( 1.0f ),
		_name( "unnamed" )
	{
	}

	inline PlotStyle PlotData::plotStyle() const
	{
		return _style;
	}

	inline void PlotData::setPlotStyle( PlotStyle style )
	{
		_style = style;
	}

	inline void PlotData::setPlotColor( const Color& c )
	{
		_color = c;
	}

	inline const Color& PlotData::plotColor() const
	{
		return _color;
	}

	inline void  PlotData::setPlotSize( float s )
	{
		_size = s;
	}

	inline float PlotData::plotSize() const
	{
		return _size;
	}

	inline void PlotData::setName( const String& name )
	{
		_name = name;
	}

	inline const String& PlotData::name() const
	{
		return _name;
	}

}

#endif
