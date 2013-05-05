/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
			void paintEvent( PaintEvent& ev, GFX& g );

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
