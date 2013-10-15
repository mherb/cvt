#ifndef CVT_PLOTDATASAMPLES_H
#define CVT_PLOTDATASAMPLES_H

#include <cvt/gui/PlotData.h>

namespace cvt {
	class PlotDataSamples : public PlotData 
	{
	 	public:
			PlotDataSamples();
			~PlotDataSamples();
	
			void addSample( const Point2f& pt );
			void clear();

			void dataInRectWithStyle( std::vector<Point2f>& data, const Rectf& rect, PlotStyle style ) const;

		private:
			std::vector<Point2f> _samples;
	};

	inline PlotDataSamples::PlotDataSamples()
	{
	}

	
	inline PlotDataSamples::~PlotDataSamples()
	{
	}

	inline void PlotDataSamples::addSample( const Point2f& pt )
	{
		_samples.push_back( pt );
		changed.notify( this );
	}

	inline void PlotDataSamples::clear()
	{
		_samples.clear();
		changed.notify( this );
	}

	inline void PlotDataSamples::dataInRectWithStyle( std::vector<Point2f>& data, const Rectf& rect, PlotStyle style ) const
	{
		if( !_samples.size() )
			return;
		switch( style ) {
			case PLOT_STYLE_LINES:
				pointsInRectToLines( data, rect, &_samples[ 0 ], _samples.size() );
				return;
			case PLOT_STYLE_DOTS:
				pointsInRect( data, rect, &_samples[ 0 ], _samples.size() );
				return;
		}
	}

};

#endif
