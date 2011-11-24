#ifndef CVT_PLOTDATA_H
#define CVT_PLOTDATA_H

#include <cvt/util/Signal.h>
#include <cvt/math/Vector.h>
#include <cvt/geom/Rect.h>
#include <cvt/gfx/Clipping.h>

namespace cvt {
	enum PlotStyle {
		PLOT_STYLE_DOTS,
		PLOT_STYLE_LINES
	};

	class PlotData {
		public:
			PlotData();
			virtual ~PlotData();

			PlotStyle plotStyle() const;
			void	  setPlotStyle( PlotStyle style );
			void	  setPlotColor( const Color& c );
			const	  Color& plotColor() const;
			void	  setPlotSize( float s );
			float	  plotSize() const;
			void      setName( const String& title );
			const     String& name() const;

			virtual void dataInRectWithStyle( std::vector<Point2f>& data, const Rectf& rect, PlotStyle style ) const = 0;

			Signal<PlotData*> changed;

		protected:
			static void pointsInRectToLines( std::vector<Point2f>& lines, const Rectf& rect, const Point2f* inpts, size_t n );
			static void pointsInRect( std::vector<Point2f>& lines, const Rectf& rect, const Point2f* inpts, size_t n );

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

	inline PlotData::~PlotData()
	{
	}

	inline PlotStyle PlotData::plotStyle() const
	{
		return _style;
	}

	inline void PlotData::setPlotStyle( PlotStyle style )
	{
		_style = style;
		changed.notify( this );
	}

	inline void PlotData::setPlotColor( const Color& c )
	{
		_color = c;
		changed.notify( this );
	}

	inline const Color& PlotData::plotColor() const
	{
		return _color;
	}

	inline void  PlotData::setPlotSize( float s )
	{
		_size = s;
		changed.notify( this );
	}

	inline float PlotData::plotSize() const
	{
		return _size;
	}

	inline void PlotData::setName( const String& name )
	{
		_name = name;
		changed.notify( this );
	}

	inline const String& PlotData::name() const
	{
		return _name;
	}

	inline void PlotData::pointsInRectToLines( std::vector<Point2f>& lines, const Rectf& rect, const Point2f* inpts, size_t n )
	{
		Vector2f pt1, pt2;
		lines.clear();
		n--;
		while( n-- ) {
			pt1 = *inpts;
			pt2 = *( inpts + 1 );
			if( Clipping::clip( rect, pt1, pt2 ) ) {
				lines.push_back( pt1 );
				lines.push_back( pt2 );
			}
			inpts++;
		}
	}

	inline void PlotData::pointsInRect( std::vector<Point2f>& pts, const Rectf& rect, const Point2f* inpts, size_t n )
	{
		pts.clear();
		while( n-- ) {
			if( rect.contains( *inpts ) ) {
				pts.push_back( *inpts );
			}
			inpts++;
		}
	}
}

#endif
