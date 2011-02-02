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
			void setImage( const Image& img );

		protected:
			void paintEvent( PaintEvent* ev, GFX* g );

		private:
			float _alpha;
			Image _img;
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
