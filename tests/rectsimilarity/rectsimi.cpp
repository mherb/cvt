#include <cvt/math/Math.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/Vector.h>
#include <cvt/geom/Line2D.h>
#include <cvt/io/Postscript.h>

#include <Eigen/Core>
#include <Eigen/LU>
#include <iostream>

using namespace cvt;

static size_t getdeltas( const Vector2f& pt1, const Vector2f& pt2,
					   const Vector2f& tpt1, const Vector2f& tpt2,
					   Vector2f* pts, Vector2f* samplepts, Vector2f* normal, float* dist, size_t spts )
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
				( *samplepts ) = pt1 + ( ( float )( i + 1 ) / ( float ) ( spts + 1 ) ) * n;
				( *normal ) = l.normal();
				*dist = l.distance( *pts );
				pts++;
				samplepts++;
				normal++;
				dist++;
				ret++;
			}
		}
	}
	return ret;
}

int main()
{
	srand( time( NULL ) );

	float angle = Math::deg2Rad( Math::rand( -20.0f, 20.0f ) );
	Matrix2f rot( Math::cos( angle ), -Math::sin( angle ), Math::sin( angle ), Math::cos( angle ) );
	float s  = 1 + Math::rand( 0.0f, 1.0f );
	float tx = Math::rand( -30.0f, 30.0f );
	float ty = Math::rand( -30.0f, 30.0f );
	Matrix3f T;

	std::cout << "Scale: " << s << std::endl;
	std::cout << "Angle: " << angle << std::endl;
	std::cout << "Tx: " << tx << std::endl;
	std::cout << "Ty: " << ty << std::endl;

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

	Vector2f pts[ 4 ], tpts[ 4 ], npts[ 4 ];
	pts[ 0 ].set( 0, 0 );
	pts[ 1 ].set( 100, 0 );
	pts[ 2 ].set( 100, 100 );
	pts[ 3 ].set( 0, 100 );


	tpts[ 0 ] = simi * pts[ 0 ];
	tpts[ 1 ] = simi * pts[ 1 ];
	tpts[ 2 ] = simi * pts[ 2 ];
	tpts[ 3 ] = simi * pts[ 3 ];

	npts[ 0 ] = tpts[ 0 ];
	npts[ 1 ] = tpts[ 1 ];
	npts[ 2 ] = tpts[ 2 ];
	npts[ 3 ] = tpts[ 3 ];

	Vector2f delta[ 4 * 4 ];
	Vector2f samplepts[ 4 * 4 ];
	Vector2f normals[ 4 * 4 ];
	float dist[ 4 * 4 ];
	size_t ndelta = 0;

	for( size_t iter = 0; iter < 5; iter++ ) {

		ndelta = 0;
		for( size_t i = 0; i < 4; i++ ) {
			ndelta += getdeltas( pts[ i ], pts[ ( i + 1 ) % 4 ],
								npts[ i ], npts[ ( i + 1 ) % 4 ],
								delta + ndelta, samplepts + ndelta, normals + ndelta, dist + ndelta, 4 );
		}


		{
			Eigen::Matrix4f A;
			Eigen::Vector4f b, tmp, x;
			Vector2f vtmp;

			A.setZero();
			b.setZero();

			for( size_t i = 0; i < ndelta; i++ ) {
				tmp( 0 ) = normals[ i ] * samplepts[ i ];
				vtmp.x = -samplepts[ i ].y;
				vtmp.y =  samplepts[ i ].x;
				tmp( 1 ) = normals[ i ] * vtmp;
				tmp( 2 ) = normals[ i ].x;
				tmp( 3 ) = normals[ i ].y;

				vtmp = delta[ i ] - samplepts[ i ];
//				std::cout << vtmp.length() << std::endl;
				b += dist[ i ] * tmp;
				A += tmp * tmp.transpose();
			};
			//x = A.inverse() * b;
			A.lu().solve(b, &x);
			float angle = - x( 1 ); //Math::deg2Rad( x( 1 ) );
			float s = 1 - x( 0 );
			float tx = -x( 2 );
			float ty = -x( 3 );
			Matrix2f rot( Math::cos( angle ), -Math::sin( angle ), Math::sin( angle ), Math::cos( angle ) );
			Matrix3f TT;
			TT[ 0 ][ 0 ] = s * rot[ 0 ][ 0 ];
			TT[ 0 ][ 1 ] = s * rot[ 0 ][ 1 ];
			TT[ 0 ][ 2 ] = tx;
			TT[ 1 ][ 0 ] = s * rot[ 1 ][ 0 ];
			TT[ 1 ][ 1 ] = s * rot[ 1 ][ 1 ];
			TT[ 1 ][ 2 ] = ty;
			TT[ 2 ][ 0 ] = 0.0f;
			TT[ 2 ][ 1 ] = 0.0f;
			TT[ 2 ][ 2 ] = 1.0f;

//			std::cout << "A:\n" <<  A << std::endl;
//			std::cout << "b:\n" << b << std::endl;
			std::cout << "x:\n" << x << std::endl;
//			std::cout << "Angle:\n" << angle << std::endl;
			std::cout << TT << std::endl;

//			TT.inverseSelf();

			npts[ 0 ] = TT * npts[ 0 ];
			npts[ 1 ] = TT * npts[ 1 ];
			npts[ 2 ] = TT * npts[ 2 ];
			npts[ 3 ] = TT * npts[ 3 ];
		}
	}


	Vector2f trans( 300, 300 );
	for( size_t i = 0; i < 4; i++ ) {
		pts[ i ] += trans;
		tpts[ i ] += trans;
		npts[ i ] += trans;
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
		g.color().set( 1.0f, 0.0f, 0.0f, 1.0f );
		g.drawLine( tpts[ 0 ], tpts[ 1 ] );
		g.drawLine( tpts[ 1 ], tpts[ 2 ] );
		g.drawLine( tpts[ 2 ], tpts[ 3 ] );
		g.drawLine( tpts[ 3 ], tpts[ 0 ] );
		g.color().set( 0.0f, 1.0f, 0.0f, 1.0f );
		g.drawLine( npts[ 0 ], npts[ 1 ] );
		g.drawLine( npts[ 1 ], npts[ 2 ] );
		g.drawLine( npts[ 2 ], npts[ 3 ] );
		g.drawLine( npts[ 3 ], npts[ 0 ] );


		g.color().set( 1.0f, 0.0f, 0.0f, 1.0f );
		for( size_t i = 0; i < ndelta; i++ ) {
			delta[ i ] += trans;
			g.fillRect( delta[ i ].x - 2, delta[ i].y - 2, 4, 4 );
		}
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
