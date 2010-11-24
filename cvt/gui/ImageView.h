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
			void setImage( const Image& img );

		protected:
			void paintEvent( PaintEvent* ev, GFX* g );

		private:
			Image _img;
	};
}

#endif
