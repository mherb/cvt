#ifndef CVT_FEATURE_VIEW_H
#define CVT_FEATURE_VIEW_H

#include <cvt/gui/ImageView.h>
#include <cvt/math/Vector.h>
#include <cvt/geom/PointSet.h>
#include <cvt/vision/features/FeatureSet.h>

class FeatureView : public cvt::ImageView
{
	public:
		FeatureView();
		~FeatureView();

		void setFeatures( const cvt::FeatureSet& features, size_t origW, size_t origH );

	private:
        cvt::PointSet2f    _points;
	protected:
		void paintEvent( cvt::PaintEvent* ev, cvt::GFX* g );
};

inline FeatureView::FeatureView() : cvt::ImageView()
{
}

inline void FeatureView::paintEvent( cvt::PaintEvent* ev, cvt::GFX* g )
{
	cvt::ImageView::paintEvent( ev, g );

	g->setColor( cvt::Color( 0.0f, 1.0f, 0.0f, 1.0f ) );
	g->drawIcons( &_points[ 0 ], _points.size(), cvt::GFX::ICON_CROSS );
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

#endif
