#include "FaceShape.h"
#include <cvt/util/Exception.h>

namespace cvt {
	FaceShape::FaceShape() : _ptsize( 0 ), _pcsize( 0 ), _lsize( 0 ),  _lines( NULL )
	{
		_transform.identity();
	}

	FaceShape::~FaceShape()
	{
		if( _lines )
			delete[] _lines;
	}

	void FaceShape::drawMean( GFX* g, const Matrix3f& transform )
	{
		Vector2f pts[ 2 ];
		size_t i1, i2;

		for( size_t i = 0; i < _lsize; i++ ) {
			i1 = _lines[ i * 2 ];
			i2 = _lines[ i * 2 + 1 ];
			pts[ 0 ].x = _mean[ i1 * 2 ];
			pts[ 0 ].y = _mean[ i1 * 2 + 1 ];
			pts[ 1 ].x = _mean[ i2 * 2 ];
			pts[ 1 ].y = _mean[ i2 * 2 + 1 ];
			pts[ 0 ] = transform * pts[ 0 ];
			pts[ 1 ] = transform * pts[ 1 ];
			g->drawLines( pts, 1 );
		}
	}

	void FaceShape::drawCurrent( GFX* g )
	{
		Vector2f pts[ 2 ];
		size_t i1, i2;

		updateCurrent();

		for( size_t i = 0; i < _lsize; i++ ) {
			i1 = _lines[ i * 2 ];
			i2 = _lines[ i * 2 + 1 ];
			pts[ 0 ].x = _pts[ i1 * 2 ];
			pts[ 0 ].y = _pts[ i1 * 2 + 1 ];
			pts[ 1 ].x = _pts[ i2 * 2 ];
			pts[ 1 ].y = _pts[ i2 * 2 + 1 ];
			g->drawLines( pts, 1 );
		}
	}

	void FaceShape::draw( GFX* g, const Matrix3f& transform, const Eigen::VectorXf& p )
	{
		Eigen::VectorXf pts( _ptsize * 2 );
		Vector2f pt[ 2 ];

		pts = _mean + _pc * p;
		for( size_t i = 0; i < _ptsize; i++ ) {
			Vector2f p;
			p.x = pts[ i * 2 ];
			p.y = pts[ i * 2 + 1 ];
			p = transform * p;
			pts[ i * 2 ] = p.x;
			pts[ i * 2 + 1 ] = p.y;
		}

		g->color().set( 1.0f, 1.0f, 1.0f, 1.0f );
		for( size_t i = 0; i < _lsize; i++ ) {
			size_t i1 = _lines[ i * 2 ];
			size_t i2 = _lines[ i * 2 + 1 ];
			pt[ 0 ].x = pts[ i1 * 2 ];
			pt[ 0 ].y = pts[ i1 * 2 + 1 ];
			pt[ 1 ].x = pts[ i2 * 2 ];
			pt[ 1 ].y = pts[ i2 * 2 + 1 ];
			g->drawLines( pt, 1 );
		}
	}

	void FaceShape::load( const char* path )
	{
		FILE* f;
		f = fopen( path, "rb" );
		if( !f )
			throw CVTException( "Could not open file!" );
		uint32_t tmp;
		fread( &tmp, sizeof( uint32_t ), 1, f );
		_ptsize = tmp;
		fread( &tmp, sizeof( uint32_t ), 1, f );
		_pcsize = tmp;

		_mean = Eigen::VectorXf::Zero( _ptsize * 2 );
		_pc = Eigen::MatrixXf::Zero( _ptsize * 2, _pcsize );
		_p = Eigen::VectorXf::Zero( _pcsize );
		_pts = Eigen::VectorXf::Zero( _ptsize * 2 );

		for( size_t i = 0; i < _ptsize * 2; i++ )
			fread( &_mean( i ), sizeof( float ), 1, f );

		for( size_t c = 0; c < _pcsize; c++ ) {
			for( size_t i = 0; i < _ptsize * 2; i++ )
				fread( &_pc( i, c ), sizeof( float ), 1, f );
		}
		fread( &tmp, sizeof( uint32_t ), 1, f );
		_lsize = tmp;
		_lines = new uint32_t[ _lsize * 2 ];
		for( size_t i = 0; i < _lsize * 2; i++ )
			fread( &_lines[ i ], sizeof( uint32_t ), 1, f );
		fclose( f );
		updateCurrent();
	}


	bool FaceShape::sampleNormal( uint8_t* ptr, uint8_t th, int _x, int _y, int x1, int y1, int x2, int y2, int n, size_t w, size_t h, size_t bpp, size_t stride, float& dist )
	{
		int dy = - Math::abs( x2 - x1 );
		int dx =   Math::abs( y2 - y1 );
		ssize_t incx, incy;
		int sx, sy;
		int x, y;
		int err = dx + dy;
		int e2;

		if( y2 < y1 ) {
			incx = -bpp;
			sx   = -1;
		} else {
			incx = bpp;
			sx   = 1;
		}

		if( x2 < x1 ) {
			incy = stride;
			sy   = 1;
		} else {
			incy = -stride;
			sy   = -1;
		}
		x = y = 0;

		uint8_t* dst1 = ptr + _x * bpp + _y * stride;
		uint8_t* dst2 = dst1;

		while( n-- ) {
			if( ( ( size_t ) ( _x + x ) ) < w && ( ( size_t ) ( _y + y ) ) < h ) {
				if( *dst1 > th ) {
					dist = Math::sqrt( ( float ) ( Math::sqr( x ) + Math::sqr( y ) ) );
					return true;
				}
			}
			if( ( ( size_t ) ( _x - x ) ) < w && ( ( size_t ) ( _y - y ) ) < h ) {
				if( *dst2 > th ) {
					dist = - Math::sqrt( ( float ) ( Math::sqr( x ) + Math::sqr( y ) ) );
					return true;
				}
			}
			e2 = 2 * err;
			if( e2 >= dy ) { err += dy; dst1 += incx; dst2 -= incx; x += sx; }
			if( e2 <= dx ) { err += dx; dst1 += incy; dst2 -= incy; y += sy; }
		}
		return false;
	}


	void FaceShape::optimize( Image& img, size_t iter )
	{
		Matrix3f tnew;
		Vector2f pts[ 2 ], n, p, ptmp;
		Eigen::VectorXf tmp;
		Eigen::MatrixXf A;
		Eigen::VectorXf b;
		uint8_t* ptr;
		size_t stride, bpp;
		float ftmp;
		size_t i1, i2;

#define OPTPC _pcsize
		A = Eigen::MatrixXf::Zero( 4 + OPTPC, 4 + OPTPC );
		b = Eigen::VectorXf::Zero( 4 + OPTPC );
		tmp = Eigen::VectorXf::Zero( 4 + OPTPC );
//		A = Eigen::MatrixXf::Zero( 4 , 4  );
//		b = Eigen::VectorXf::Zero( 4  );
//		tmp = Eigen::VectorXf::Zero( 4  );

		bpp = img.bpp();
		ptr = img.map<uint8_t>( &stride );

#define MAXDIST 10

		while( iter-- ) {
			updateCurrent();
			for( size_t i = 0; i < _lsize; i++ ) {
				i1 = _lines[ i * 2 ];
				i2 = _lines[ i * 2 + 1 ];

				pts[ 0 ].x = _pts[ i1 * 2 ];
				pts[ 0 ].y = _pts[ i1 * 2 + 1 ];
				pts[ 1 ].x = _pts[ i2 * 2 ];
				pts[ 1 ].y = _pts[ i2 * 2 + 1 ];
				Vector2f n = pts[ 1 ] - pts[ 0 ];
				n.normalize();
				ftmp = -n.x;
				n.x = n.y;
				n.y = ftmp;

				Matrix2f Ttmp( _transform );
				for( float alpha = 0.0f; alpha <= 1.0f; alpha += 0.025f ) {
					p = Math::mix( pts[ 0 ], pts[ 1 ], alpha );
					tmp( 0 ) = n * p;
					tmp( 1 ) = - n.x * p.y + n.y * p.x;
					tmp( 2 ) = p.x;
					tmp( 3 ) = p.y;
					for( size_t k = 0; k < OPTPC; k++ ) {
						ptmp.x = Math::mix( _pc( i1 * 2, k ), _pc( i2 * 2, k ), alpha );
						ptmp.y = Math::mix( _pc( i1 * 2 + 1, k ), _pc( i2 * 2 + 1, k ), alpha );
						ptmp = Ttmp * ptmp;
						tmp( 4 + k ) = n * ptmp;
					}
					if( sampleNormal( ptr, 0xff >> 1, Math::round( p.x ), Math::round( p.y ),
									 Math::round( pts[ 0 ].x ), Math::round( pts[ 0 ].y ), Math::round( pts[ 1 ].x ), Math::round( pts[ 1 ].y ), MAXDIST, img.width(), img.height(), bpp, stride, ftmp ) ) {
						A += tmp * tmp.transpose();
						b += tmp * ftmp;
					}
				}

			}

//			std::cout << "A:\n" << A << std::endl;
//			std::cout << "b:\n" << b << std::endl;
			tmp = Eigen::VectorXf::Zero( tmp.rows() );
//			tmp = A.inverse() * b;
			A.lu().solve( b, &tmp );
//			std::cout << tmp << std::endl;
			float angle = tmp( 1 ); //Math::deg2Rad( x( 1 ) );
			float s = 1 + tmp( 0 );
			float tx = tmp( 2 );
			float ty = tmp( 3 );
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

			_transform = TT * _transform;
			_p.block( 0, 0, OPTPC, 1 ) += tmp.block( 4, 0, OPTPC, 1 );
		}
		img.unmap( ptr );
	}

	void FaceShape::updateCurrent()
	{
		Vector2f pt;
		_pts = _mean + _pc * _p;
		for( size_t i = 0; i < _ptsize; i++ ) {
			pt.x = _pts[ i * 2 ];
			pt.y = _pts[ i * 2 + 1 ];
			pt = _transform * pt;
			_pts[ i * 2 ] = pt.x;
			_pts[ i * 2 + 1 ] = pt.y;
		}
	}
}
