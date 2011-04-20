#ifndef CVT_FEATURE_VIEW_H
#define CVT_FEATURE_VIEW_H

#include <cvt/gui/ImageView.h>
#include <cvt/vision/FeatureExtractor.h>
#include <cvt/math/Vector.h>

class FeatureView : public cvt::ImageView
{
	public:
		FeatureView();
		~FeatureView();

		void setFeatures( std::vector<cvt::Feature2D> & features, size_t origW, size_t origH );

	private:
        cvt::Vector2i*    _features;
        size_t            _numFeatures;
	protected:
		void paintEvent( cvt::PaintEvent* ev, cvt::GFX* g );
};

inline FeatureView::FeatureView() : cvt::ImageView(), _features( 0 ), _numFeatures( 0 )
{
}

void FeatureView::paintEvent( cvt::PaintEvent* ev, cvt::GFX* g )
{
	cvt::ImageView::paintEvent( ev, g );

	g->setColor( cvt::Color( 0.0f, 1.0f, 0.0f, 1.0f ) );

	g->drawIcons( _features, _numFeatures, cvt::GFX::ICON_CROSS );
}

inline FeatureView::~FeatureView()
{
    if( _features )
        delete _features;
}

inline void FeatureView::setFeatures( std::vector<cvt::Feature2D> & features, size_t origW, size_t origH )
{
	if( _numFeatures != features.size() ){
        delete[] _features;
        _numFeatures = features.size();
        _features = new cvt::Vector2i[ _numFeatures ];
    }

    int w, h;
	size( w, h );

    float xScale = ( float ) w / origW;
	float yScale = ( float ) h / origH;

	for( size_t i = 0; i < features.size(); i++ ){
		_features[ i ].x = xScale * features[ i ][ 0 ];
		_features[ i ].y = yScale * features[ i ][ 1 ];
	}
}

#endif
