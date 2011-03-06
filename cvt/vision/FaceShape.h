#ifndef CVT_FACESHAPE_H
#define CVT_FACESHAPE_H


#include <cvt/gfx/Image.h>
#include <cvt/vision/MeasurementModel.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/CostFunction.h>
#include <cvt/util/Resources.h>
#include <stdio.h>

namespace cvt {
	template<typename T>
	class FaceShape : public MeasurementModel< T,
				  FaceShape,
				  Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>,
				  Eigen::Matrix<T, Eigen::Dynamic, 1>,
				  Eigen::Matrix<T, Eigen::Dynamic, 1>,
				  T >
	{
		public:
			FaceShape();
			~FaceShape();
			void	updateInput( const Image * img ){ _currI = img; updateInputData(); };
			void	setTransform( T scale, T angle, T tx, T ty );

			void	apply( const Eigen::Matrix<T, Eigen::Dynamic, 1> & delta );
			T		buildLSSystem( Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> & A, Eigen::Matrix<T, Eigen::Dynamic, 1> & b, const CostFunction<T, T> & costFunc );
			T		evaluateCosts( const CostFunction<T, T> & costFunc );

		private:
			void updateInputData();
			void updateCurrent();
			void loadFaceShape( const char* path );

			Image* _currI;
			IKernel _kdx, _kdy;
			Image _dx;
			Image _dy;
			size_t _ptsize;
			size_t _pcsize;
			size_t _lsize;
			uint32_t* _lines;
			T _costs;
			Matrix3<T> _transform;
			Eigen::Matrix<T, Eigen::Dynamic, 1 > _p;
			Eigen::Matrix<T, Eigen::Dynamic, 1 > _mean;
			Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic > _pc;
			Eigen::Matrix<T, Eigen::Dynamic, 1 > _pts;
	};

	template<typename T>
	inline FaceShape<T>::FaceShape() : _currI( NULL), _kdx( IKernel::HAAR_HORIZONTAL_3 ), _kdy( IKernel::HAAR_VERTICAL_3 ),
									  _ptsize( 0 ), _pcsize( 0 ), _lsize( 0 ), _lines( 0 ), _costs( 0 )
	{
		_transform.identity();

		std::string dataFolder(DATA_FOLDER);
		std::string inputFile(dataFolder + "/face.data");
		loadFaceShape( inputFile.c_str() );
	}

	template<typename T>
	inline FaceShape<T>::~FaceShape()
	{
		delete[] _lines;
	}

	template<typename T>
	inline void FaceShape<T>::loadFaceShape( const char* path )
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

		_mean = Eigen::Matrix<T, Eigen::Dynamic, 1>::Zero( _ptsize * 2 );
		_pc = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>::Zero( _ptsize * 2, _pcsize );
		_p = Eigen::Matrix<T, Eigen::Dynamic, 1>::Zero( _pcsize );
		_pts = Eigen::Matrix<T, Eigen::Dynamic, 1>::Zero( _ptsize * 2 );

		float ftmp;
		for( size_t i = 0; i < _ptsize * 2; i++ ) {
			fread( &ftmp, sizeof( float ), 1, f );
			_mean( i ) = ftmp
		}

		for( size_t c = 0; c < _pcsize; c++ ) {
			for( size_t i = 0; i < _ptsize * 2; i++ ) {
				fread( &ftmp, sizeof( float ), 1, f );
				_pc( i, c ) = ftmp;
			}
		}
		fread( &tmp, sizeof( uint32_t ), 1, f );
		_lsize = tmp;
		_lines = new uint32_t[ _lsize * 2 ];
		for( size_t i = 0; i < _lsize * 2; i++ )
			fread( &_lines[ i ], sizeof( uint32_t ), 1, f );
		fclose( f );
		updateCurrent();
	}

	template<typename T>
	inline void FaceShape<T>::setTransform( T scale, T angle, T tx, T ty )
	{
		T angle = Math::deg2Rad( angle );
		T s = scale;
		T tx = tx;
		T ty = ty;
		Matrix2f rot( Math::cos( angle ), -Math::sin( angle ), Math::sin( angle ), Math::cos( angle ) );
		_transform[ 0 ][ 0 ] = s * rot[ 0 ][ 0 ];
		_transform[ 0 ][ 1 ] = s * rot[ 0 ][ 1 ];
		_transform[ 0 ][ 2 ] = tx;
		_transform[ 1 ][ 0 ] = s * rot[ 1 ][ 0 ];
		_transform[ 1 ][ 1 ] = s * rot[ 1 ][ 1 ];
		_transform[ 1 ][ 2 ] = ty;
		_transform[ 2 ][ 0 ] = 0.0f;
		_transform[ 2 ][ 1 ] = 0.0f;
		_transform[ 2 ][ 2 ] = 1.0f;
	}

	template<T>
	inline void FaceShape<T>::updateCurrent()
	{
		Vector2<T> pt;
		_pts = _mean + _pc * _p;
		for( size_t i = 0; i < _ptsize; i++ ) {
			pt.x = _pts[ i * 2 ];
			pt.y = _pts[ i * 2 + 1 ];
			pt = _transform * pt;
			_pts[ i * 2 ] = pt.x;
			_pts[ i * 2 + 1 ] = pt.y;
		}
	}

	template<typename T>
	inline void FaceShape<T>::updateInputData()
	{
		_dx.reallocate( *_currI );
		_dy.reallocate( *_currI );
		_currI->convolve( _dx, _kdx );
		_currI->convolve( _dy, _kdy );
	}


	template<typename T>
	inline void	FaceShape<T>::apply( const Eigen::Matrix<T, Eigen::Dynamic, 1> & delta )
	{
		T angle = delta( 1 );
		T s = 1 + delta( 0 );
		T tx = delta( 2 );
		T ty = delta( 3 );
		Matrix2<T> rot( Math::cos( angle ), -Math::sin( angle ), Math::sin( angle ), Math::cos( angle ) );
		Matrix3<T> TT;
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
		_p.block( 0, 0, _pcsize, 1 ) += tmp.block( 4, 0, _pcsize, 1 );
		updateCurrent();
	}


	template<typename T>
	inline T FaceShape<T>::buildLSSystem( Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> & A, Eigen::Matrix<T, Eigen::Dynamic, 1> & b, const CostFunction<T, T> & costFunc )
	{
		Matrix3<T> tnew;
		Vector2<T> pts[ 2 ], n, p, ptmp;
		Eigen::Matrix<T, Eigen::Dynamic, 1> tmp;
		uint8_t* dxptr;
		uint8_t* dyptr;
		size_t dxstride, dystride, dxbpp, dybpp;
		float ftmp;
		size_t i1, i2;
		T weight;

		A.setZero();
		b.setZero();

		dxbpp = _dx.bpp();
		dxptr = _dx.map<uint8_t>( &dxstride );
		dybpp = _dx.bpp();
		dyptr = _dy.map<uint8_t>( &dystride );


#define MAXDIST 10
#define INCR	0.1f

		_costs = 0;

		for( size_t i = 0; i < _lsize; i++ ) {
			i1 = _lines[ i * 2 ];
			i2 = _lines[ i * 2 + 1 ];

			pts[ 0 ].x = _pts[ i1 * 2 ];
			pts[ 0 ].y = _pts[ i1 * 2 + 1 ];
			pts[ 1 ].x = _pts[ i2 * 2 ];
			pts[ 1 ].y = _pts[ i2 * 2 + 1 ];
			Vector2<T> n = pts[ 1 ] - pts[ 0 ];
			n.normalize();
			ftmp = -n.x;
			n.x = n.y;
			n.y = ftmp;

			Matrix2<T> Ttmp( _transform );
			for( T alpha = 0; alpha <= 1; alpha += INCR ) {
				p = Math::mix( pts[ 0 ], pts[ 1 ], alpha );
				tmp( 0 ) = n * p;
				tmp( 1 ) = - n.x * p.y + n.y * p.x;
				tmp( 2 ) = p.x;
				tmp( 3 ) = p.y;
				for( size_t k = 0; k < _pcsize; k++ ) {
					ptmp.x = Math::mix( _pc( i1 * 2, k ), _pc( i2 * 2, k ), alpha );
					ptmp.y = Math::mix( _pc( i1 * 2 + 1, k ), _pc( i2 * 2 + 1, k ), alpha );
					ptmp = Ttmp * ptmp;
					tmp( 4 + k ) = n * ptmp;
				}
//				if( sampleNormal( ptr, 0xff >> 1, Math::round( p.x ), Math::round( p.y ),
//								 Math::round( pts[ 0 ].x ), Math::round( pts[ 0 ].y ), Math::round( pts[ 1 ].x ), Math::round( pts[ 1 ].y ), MAXDIST, img.width(), img.height(), bpp, stride, ftmp ) ) {
					_costs += costFunc.cost( ftmp, weight );

					if( costFunc.isRobust ){
						A += tmp * weight * tmp.transpose();
						b += tmp * ftmp * weight;
					} else {
						A += tmp * tmp.transpose();
						b += tmp * ftmp;
					}
//				}
			}
		}
		img.unmap( dxptr );
		img.unmap( dyptr );
	}
}

