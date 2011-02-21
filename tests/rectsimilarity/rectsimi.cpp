#include <cvt/math/Math.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/Vector.h>
#include <cvt/geom/Line2D.h>
#include <cvt/io/Postscript.h>

#include <iostream>

using namespace cvt;

static size_t getdeltas( const Vector2f& pt1, const Vector2f& pt2,
					   const Vector2f& tpt1, const Vector2f& tpt2,
					   Vector2f* pts, size_t spts )
{
	size_t ret = 0;
	Line2Df l( pt1, pt2 );
	Line2Df tl( tpt1, tpt2 );
	Line2Df o;
	Vector2f n = pt2 - pt1;
	Vector2f min, max;

	min.x = Math::min( tpt1.x, tpt2.x );
	min.y = Math::min( tpt1.y, tpt2.y );
	max.x = Math::max( tpt1.x, tpt2.x );
	max.y = Math::max( tpt1.y, tpt2.y );

	for( size_t i = 0; i < spts; i++ ) {
		o.setOrthogonal( l, pt1 + ( ( float )( i + 1 ) / ( float ) ( spts + 1 ) ) * n );
		if( tl.intersect( o, *pts ) ) {
			if( ( *pts ).x >= min.x && ( *pts ).x <= max.x &&
			   ( *pts ).y >= min.y && ( *pts ).y <= max.y ) {
				std::cout << ( *pts ) << std::endl;
				pts++;
				ret++;
			}
		}
	}
	return ret;
}

int main()
{
	srand( time( NULL ) );

	float angle = Math::deg2Rad( Math::rand( -10.0f, 10.0f ) );
	Matrix2f rot( Math::cos( angle ), -Math::sin( angle ), Math::sin( angle ), Math::cos( angle ) );
	float s  = 1 + Math::rand( 0.0f, 0.5f );
	float tx = Math::rand( -25.0f, 25.0f );
	float ty = Math::rand( -25.0f, 25.0f );

	Matrix3f simi;
	simi[ 0 ][ 0 ] = s * rot[ 0 ][ 0 ];
	simi[ 0 ][ 1 ] = s * rot[ 0 ][ 1 ];
	simi[ 0 ][ 2 ] = tx;
	simi[ 1 ][ 0 ] = s * rot[ 1 ][ 0 ];
	simi[ 1 ][ 1 ] = s * rot[ 1 ][ 1 ];
	simi[ 1 ][ 2 ] = ty;
	simi[ 2 ][ 0 ] = 0.0f;
	simi[ 2 ][ 1 ] = 0.0f;
	simi[ 2 ][ 2 ] = 1.0f;

	Vector2f pts[ 4 ], tpts[ 4 ];
	pts[ 0 ].set( 0, 0 );
	pts[ 1 ].set( 100, 0 );
	pts[ 2 ].set( 100, 100 );
	pts[ 3 ].set( 0, 100 );


	tpts[ 0 ] = simi * pts[ 0 ];
	tpts[ 1 ] = simi * pts[ 1 ];
	tpts[ 2 ] = simi * pts[ 2 ];
	tpts[ 3 ] = simi * pts[ 3 ];
	Vector2f trans( 300, 300 );


	Vector2f delta[ 4 * 4 ];
	size_t ndelta = 0;
	for( size_t i = 0; i < 4; i++ ) {
		ndelta += getdeltas( pts[ i ], pts[ i % 4 ],
			   tpts[ i ], tpts[ i % 4 ],
			   delta + ndelta, 4 );
	}

	for( size_t i = 0; i < ndelta; i++ )
		std::cout << delta[ i ] << std::endl;

	for( size_t i = 0; i < 4; i++ ) {
		pts[ i ] += trans;
		tpts[ i ] += trans;
	}


	{
		Postscript ps("out.ps");
		GFX g( &ps );
		g.color().set( 0.0f, 0.0f, 0.0f, 1.0f );
		g.setLineWidth( 1 );
		g.drawLine( pts[ 0 ], pts[ 1 ] );
		g.drawLine( pts[ 1 ], pts[ 2 ] );
		g.drawLine( pts[ 2 ], pts[ 3 ] );
		g.drawLine( pts[ 3 ], pts[ 0 ] );
		g.color().set( 0.0f, 1.0f, 0.0f, 1.0f );
		g.drawLine( tpts[ 0 ], tpts[ 1 ] );
		g.drawLine( tpts[ 1 ], tpts[ 2 ] );
		g.drawLine( tpts[ 2 ], tpts[ 3 ] );
		g.drawLine( tpts[ 3 ], tpts[ 0 ] );
	}

/*	getdeltas( Vector2f( 0, 0 ), Vector2f( 10, 0 ),
			   Vector2f( 0, 10 ), Vector2f( 10, 10 ),
			   delta, 4 );

	std::cout << delta[ 0 ] << std::endl;
	std::cout << delta[ 1 ] << std::endl;
	std::cout << delta[ 2 ] << std::endl;
	std::cout << delta[ 3 ] << std::endl;*/

	return 0;
}
