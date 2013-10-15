#ifndef CVT_IMAGEVIEW_H
#define CVT_IMAGEVIEW_H

#include <cvt/gui/Widget.h>
#include <cvt/gfx/Image.h>

namespace cvt {
	class ImageView : public Widget
	{
		public:
			ImageView();
			~ImageView();
			void setAlpha( float a );
			float alpha() const;
			void setFixAspect( bool val );
			void setImage( const Image& img );

		protected:
			void paintEvent( PaintEvent& ev, GFX& g );

			float _alpha;
			Image _img;
			bool  _fixAspect;
	};

	inline void ImageView::setAlpha( float a )
	{
		_alpha = a;
		update();
	}

	inline float ImageView::alpha() const
	{
		return _alpha;
	}

}

#endif
