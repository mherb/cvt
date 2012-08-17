#include <cvt/gui/ImageView.h>
#include <cvt/geom/Rect.h>

namespace cvt {

	class RectSelectImageView : public ImageView
	{
		public:
			RectSelectImageView(); 
			~RectSelectImageView(); 

			void mouseMoveEvent( MouseMoveEvent* e );
			void mousePressEvent( MousePressEvent* e );
			void mouseReleaseEvent( MouseReleaseEvent* e );
			void paintEvent( PaintEvent* e , GFX* gfx );
			
			// update the points that shall be drawn!
			void updatePoints( const std::vector<Vector2f> & pts );

			Signal<const Rectf&>	selectionComplete;
			Signal<void>			selectionDidStart;

		private:
			Vector2f _firstClick;
			
			// stored in normalized dimensions, s.t. we can always scale with our current dimensions!
			Vector2f _pts[ 4 ];
			void updateSelection( float x, float y );
	};

	inline RectSelectImageView::RectSelectImageView() : ImageView()
	{
		_pts[ 0 ][ 0 ] = 0;
		_pts[ 0 ][ 1 ] = 0;
		_pts[ 1 ][ 0 ] = 0;
		_pts[ 1 ][ 1 ] = 0;
		_pts[ 2 ][ 0 ] = 0;
		_pts[ 2 ][ 1 ] = 0;
		_pts[ 3 ][ 0 ] = 0;
		_pts[ 3 ][ 1 ] = 0;
	}

	inline RectSelectImageView::~RectSelectImageView()
	{
	}

	inline void RectSelectImageView::paintEvent( PaintEvent* e , GFX* gfx )
	{
		ImageView::paintEvent( e, gfx );

		int w, h;

		this->size( w, h );
		gfx->color().set( 0.0f, 1.0f, 0.0f, 1.0f );		
		gfx->setLineWidth( 5 );
		gfx->drawLine( w* _pts[ 0 ][ 0 ], h* _pts[ 0 ][ 1 ], w* _pts[ 1 ][ 0 ], h* _pts[ 1 ][ 1 ] );
		gfx->drawLine( w* _pts[ 1 ][ 0 ], h* _pts[ 1 ][ 1 ], w* _pts[ 2 ][ 0 ], h* _pts[ 2 ][ 1 ] );
		gfx->drawLine( w* _pts[ 2 ][ 0 ], h* _pts[ 2 ][ 1 ], w* _pts[ 3 ][ 0 ], h* _pts[ 3 ][ 1 ] );
		gfx->drawLine( w* _pts[ 3 ][ 0 ], h* _pts[ 3 ][ 1 ], w* _pts[ 0 ][ 0 ], h* _pts[ 0 ][ 1 ] );
	}


	inline void RectSelectImageView::updatePoints( const std::vector<Vector2f> & pts )
	{
		for( size_t i = 0; i < 4; i++ ){
			_pts[ i ].x = pts[ i ].x;
			_pts[ i ].y = pts[ i ].y;
		}
		update();
	}

	inline void RectSelectImageView::updateSelection( float x, float y )
	{
		_pts[ 0 ][ 0 ] = _firstClick[ 0 ];
		_pts[ 0 ][ 1 ] = _firstClick[ 1 ];
		_pts[ 1 ][ 0 ] = x;
		_pts[ 1 ][ 1 ] = _firstClick[ 1 ];
		_pts[ 2 ][ 0 ] = x;
		_pts[ 2 ][ 1 ] = y;
		_pts[ 3 ][ 0 ] = _firstClick[ 0 ];
		_pts[ 3 ][ 1 ] = y;
	}

	inline void RectSelectImageView::mouseMoveEvent( MouseMoveEvent* e )
	{
		if( ! ( e->buttonMask() & 1 ) ) return;

		int w, h;
		this->size( w, h );
		float x = ( float )e->x / ( float )w;
		float y = ( float )e->y / ( float )h;
		updateSelection( x, y );			
	}

	inline void RectSelectImageView::mousePressEvent( MousePressEvent* e )
	{
		if( e->button() != 1 ) return;		

		int w, h;
		this->size( w, h );

		_firstClick[ 0 ] = ( float )( e->x ) / ( float )w;
		_firstClick[ 1 ] = ( float )( e->y ) / ( float )h;

		updateSelection( _firstClick[ 0 ] , _firstClick[ 1 ] );
		selectionDidStart.notify();
	}

	inline void RectSelectImageView::mouseReleaseEvent( MouseReleaseEvent* e )
	{
		if( e->button() != 1 ) return;

		int w, h;
		this->size( w, h );
		float x = ( float )e->x / ( float )w;
		float y = ( float )e->y / ( float )h;
		updateSelection( x, y );


		Rectf r;
		r.x = Math::min( _pts[ 0 ][ 0 ], _pts[ 2 ][ 0 ] );
		r.y = Math::min( _pts[ 0 ][ 1 ], _pts[ 2 ][ 1 ] );
		r.width = Math::abs( _pts[ 1 ][ 0 ] - _pts[ 0 ][ 0 ] );
		r.height = Math::abs( _pts[ 1 ][ 1 ] - _pts[ 2 ][ 1 ] );

		selectionComplete.notify( r );
	}


}
