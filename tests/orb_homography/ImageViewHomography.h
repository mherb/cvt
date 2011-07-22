#ifndef CVT_IMAGEVIEWHOMOGRAPHY_H
#define CVT_IMAGEVIEWHOMOGRAPHY_H

#include <cvt/gui/ImageView.h>

namespace cvt {

	class ImageViewHomography : public ImageView
	{

		public:
			ImageViewHomography() : ImageView()
			{}

			void paintEvent( PaintEvent* e , GFX* gfx ) 
			{
				ImageView::paintEvent( e, gfx );

				int w, h;

				this->size( w, h );
				gfx->color().set( 0.0f, 1.0f, 0.0f, 1.0f );		
				gfx->setLineWidth( 5 );
				gfx->drawLine( w * _pts[ 0 ][ 0 ], h * _pts[ 0 ][ 1 ], w * _pts[ 1 ][ 0 ], h * _pts[ 1 ][ 1 ] );
				gfx->drawLine( w * _pts[ 1 ][ 0 ], h * _pts[ 1 ][ 1 ], w * _pts[ 2 ][ 0 ], h * _pts[ 2 ][ 1 ] );
				gfx->drawLine( w * _pts[ 2 ][ 0 ], h * _pts[ 2 ][ 1 ], w * _pts[ 3 ][ 0 ], h * _pts[ 3 ][ 1 ] );
				gfx->drawLine( w * _pts[ 3 ][ 0 ], h * _pts[ 3 ][ 1 ], w * _pts[ 0 ][ 0 ], h * _pts[ 0 ][ 1 ] );
			}

			void setPoints( const Vector2f& pt0, const Vector2f& pt1, const Vector2f& pt2, const Vector2f& pt3 )
			{
				_pts[ 0 ] = pt0;
				_pts[ 1 ] = pt1;
				_pts[ 2 ] = pt2;
				_pts[ 3 ] = pt3;
				update();
			}

		private:
			// normalized dimensions!
			Vector2f _pts[ 4 ];

	};
}

#endif
