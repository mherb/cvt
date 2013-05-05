#ifndef CVT_FEATURE_VIEW_H
#define CVT_FEATURE_VIEW_H

#include <cvt/gui/ImageView.h>
#include <cvt/math/Vector.h>
#include <cvt/geom/PointSet.h>
#include <cvt/vision/features/FeatureSet.h>
#include <cvt/vision/features/FeatureMatch.h>

class FeatureView : public cvt::ImageView
{
	public:
		FeatureView();
		~FeatureView();

		void setFeatures( const cvt::FeatureSet& features, size_t origW, size_t origH );
		void setTracks( const std::vector<cvt::FeatureMatch>& features, size_t origW, size_t origH );

	private:
        cvt::PointSet2f    _points;
        cvt::PointSet2f    _tracks;
	protected:
		void paintEvent( cvt::PaintEvent& ev, cvt::GFX& g );
};

inline FeatureView::FeatureView() : cvt::ImageView()
{
}

inline void FeatureView::paintEvent( cvt::PaintEvent& ev, cvt::GFX& g )
{
	cvt::ImageView::paintEvent( ev, g );

	g.setColor( cvt::Color( 0.0f, 1.0f, 0.0f, 1.0f ) );
	g.drawIcons( &_points[ 0 ], _points.size(), cvt::GFX::ICON_CROSS );

	g.setColor( cvt::Color( 0.0f, 0.0f, 1.0f, 1.0f ) );
	g.drawLines( &_tracks[ 0 ], _tracks.size() );
}

inline FeatureView::~FeatureView()
{
}

inline void FeatureView::setFeatures( const cvt::FeatureSet& features, size_t origW, size_t origH )
{

	features.toPointSet2f( _points );

    int w, h;
	size( w, h );

    float xScale = ( float ) w / origW;
	float yScale = ( float ) h / origH;

	for( size_t i = 0; i < _points.size(); i++ ){
		_points[ i ].x *= xScale;
		_points[ i ].y *= yScale;
	}
	update();
}

inline void FeatureView::setTracks( const std::vector<cvt::FeatureMatch>& matches, size_t origW, size_t origH )
{
    int w, h;
	size( w, h );

    float xScale = ( float ) w / origW;
	float yScale = ( float ) h / origH;

	_tracks.clear();

	for( size_t i = 0; i < matches.size(); i++ ){
		cvt::Point2f pt;
		pt = matches[ i ].feature0->pt;
		pt.x *= xScale;
		pt.y *= yScale;
		_tracks.add( pt );
		pt = matches[ i ].feature1->pt;
		pt.x *= xScale;
		pt.y *= yScale;
		_tracks.add( pt );
	}
	update();
}

#endif
