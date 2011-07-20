#ifndef CVT_POINTSET_H
#define CVT_POINTSET_H

#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/util/Exception.h>
#include <vector>
#include <Eigen/SVD>

namespace cvt
{
	template<int dim, typename _T>	class PointSet;

	typedef PointSet<2,float> PointSet2f;
	typedef PointSet<3,float> PointSet3f;
	typedef PointSet<2,double> PointSet2d;
	typedef PointSet<3,double> PointSet3d;

	template<int dim, typename _T>
		class PointSet
		{
			typedef typename Vector<dim,_T>::TYPE PTTYPE;
			typedef typename Matrix<dim,_T>::TYPE MATTYPENT;
			typedef typename Matrix<dim + 1,_T>::TYPE MATTYPE;

			public:
				PointSet();
				~PointSet();
				PointSet( const PointSet<dim,_T>& ptset );
				PointSet( const _T* data, size_t npts );
				void add( const PTTYPE& pt );
				void clear();
				PTTYPE& operator[]( int i );
				const PTTYPE& operator[]( int i ) const;
				PTTYPE mean() const;
				PTTYPE variance() const;
				size_t size() const;
				void resize( size_t );
				void translate( const PTTYPE& t );
				void scale( _T t );
				void transform( const MATTYPENT& mat );
				void transform( const MATTYPE& mat );
				void normalize( MATTYPE& mat );
				void normalize( );
				_T	 ssd( const PointSet<dim, _T>& ptset ) const;
				_T	 maxSquaredDistance( const PointSet<dim, _T>& ptset ) const;
				MATTYPE alignSimilarity( const PointSet<dim,_T>& ptset ) const;
				Matrix3<_T> alignPerspective( const PointSet<dim,_T>& ptset ) const;
				const _T* ptr() const;

			private:
				std::vector<PTTYPE>	_pts;
		};

	template<int dim, typename _T>
		inline PointSet<dim,_T>::PointSet()
		{
		}

	template<int dim, typename _T>
		inline PointSet<dim,_T>::~PointSet()
		{
		}

	template<int dim, typename _T>
		inline PointSet<dim,_T>::PointSet( const PointSet<dim,_T>& ptset ) : _pts( ptset._pts )
	{
	}

	template<int dim, typename _T>
		inline PointSet<dim,_T>::PointSet( const _T* data, size_t npts )
		{
			resize( npts );
			SIMD::instance()->Memcpy( ( uint8_t* ) &_pts[ 0 ], ( uint8_t* ) data, npts * sizeof( _T ) * dim );
		}

	template<int dim, typename _T>
		inline void PointSet<dim,_T>::clear()
		{
			_pts.clear();
		}

	template<int dim, typename _T>
		inline void PointSet<dim,_T>::add( const PTTYPE& pt )
		{
			_pts.push_back( pt );
		}

	template<int dim, typename _T>
		inline typename PointSet<dim, _T>::PTTYPE& PointSet<dim,_T>::operator[]( int i )
		{
			return _pts[ i ];
		}

	template<int dim, typename _T>
		inline const typename PointSet<dim,_T>::PTTYPE& PointSet<dim,_T>::operator[]( int i ) const
		{
			return _pts[ i ];
		}

	template<int dim, typename _T>
		inline	typename PointSet<dim,_T>::PTTYPE PointSet<dim,_T>::mean() const
		{
			const PTTYPE* pt = &_pts[ 0 ];
			PTTYPE mean;
			size_t n = _pts.size();

			mean.zero();
			while( n-- )
				mean += *pt++;

			mean /= ( _T ) size();
			return mean;
		}

	template<int dim, typename _T>
		inline	typename PointSet<dim,_T>::PTTYPE PointSet<dim,_T>::variance() const
		{
			const PTTYPE* pt = &_pts[ 0 ];
			size_t n = size();
			PTTYPE mu = mean();
			PTTYPE var;

			var.zero();
			while( n-- )
				var += ( *pt++ - mu ).lengthSqr();

			var /= ( _T ) size();
			return var;
		}


	template<int dim, typename _T>
		inline void PointSet<dim,_T>::translate( const PTTYPE& t )
		{
			PTTYPE* pt = &_pts[ 0 ];
			size_t n = _pts.size();
			while( n-- )
				*pt++ -= t;
		}

	template<int dim, typename _T>
		inline void PointSet<dim,_T>::scale( _T s )
		{
			PTTYPE* pt = &_pts[ 0 ];
			size_t n = _pts.size();
			while( n-- )
				*pt++ *= s;
		}

	template<int dim, typename _T>
		inline void PointSet<dim,_T>::transform( const MATTYPENT& mat )
		{
			PTTYPE* pt = &_pts[ 0 ];
			size_t n = _pts.size();
			while( n-- ) {
				*pt = mat * *pt;
				pt++;
			}
		}


	template<>
		inline void PointSet<2,float>::transform( const MATTYPE& mat )
		{
			PTTYPE* pt = &_pts[ 0 ];
			size_t n = _pts.size();

			/* if last row is [ 0 0 1 ], split mat into 2 x 2 matrix and translation */
			if( mat[ 2 ] == Vector3f( 0, 0, 1.0f ) ) {
				Matrix2f _mat( mat );
				Vector2f trans( mat[ 0 ][ 2 ], mat[ 1 ][ 2 ] );
				while( n-- ) {
					*pt = _mat * *pt;
					*pt += trans;
					pt++;
				}
			} else {
				while( n-- ) {
					*pt = mat * *pt;
					pt++;
				}
			}
		}

	template<>
		inline void PointSet<2,double>::transform( const MATTYPE& mat )
		{
			PTTYPE* pt = &_pts[ 0 ];
			size_t n = _pts.size();

			/* if last row is [ 0 0 1 ], split mat into 2 x 2 matrix and translation */
			if( mat[ 2 ] == Vector3d( 0, 0, 1.0 ) ) {
				Matrix2d _mat( mat );
				Vector2d trans( mat[ 0 ][ 2 ], mat[ 1 ][ 2 ] );
				while( n-- ) {
					*pt = _mat * *pt;
					*pt += trans;
					pt++;
				}
			} else {
				while( n-- ) {
					*pt = mat * *pt;
					pt++;
				}
			}
		}

	template<>
		inline void PointSet<3,float>::transform( const MATTYPE& mat )
		{
			PTTYPE* pt = &_pts[ 0 ];
			size_t n = _pts.size();

			/* if last row is [ 0 0 0 1 ], split mat into 3 x 3 matrix and translation */
			if( mat[ 3 ] == Vector4f( 0, 0, 0, 1 ) ) {
				Matrix3f _mat( mat );
				Vector3f trans( mat[ 0 ][ 3 ], mat[ 1 ][ 3 ], mat[ 2 ][ 3 ] );
				while( n-- ) {
					*pt = _mat * *pt;
					*pt += trans;
					pt++;
				}
			} else {
				while( n-- ) {
					*pt = mat * *pt;
					pt++;
				}
			}
		}

	template<>
		inline void PointSet<3,double>::transform( const MATTYPE& mat )
		{
			PTTYPE* pt = &_pts[ 0 ];
			size_t n = _pts.size();

			/* if last row is [ 0 0 0 1 ], split mat into 3 x 3 matrix and translation */
			if( mat[ 3 ] == Vector4d( 0, 0, 0, 1 ) ) {
				Matrix3d _mat( mat );
				Vector3d trans( mat[ 0 ][ 3 ], mat[ 1 ][ 3 ], mat[ 2 ][ 3 ] );
				while( n-- ) {
					*pt = _mat * *pt;
					*pt += trans;
					pt++;
				}
			} else {
				while( n-- ) {
					*pt = mat * *pt;
					pt++;
				}
			}
		}

	template<int dim, typename _T>
		inline void PointSet<dim,_T>::normalize( MATTYPE& mat )
		{
			PTTYPE* pt = &_pts[ 0 ];
			PTTYPE m = mean();
			_T s = 0;
			size_t n = size();


			/* remove mean */
			while( n-- ) {
				*pt -= m;
				s += pt->length();
				pt++;
			}

			/* scale */
			s = ( Math::sqrt( ( _T ) 2 ) * ( _T ) size() ) / s;
			scale( s );

			mat.setIdentity();
			for( int i = 0; i < mat.dimension() - 1; i++ ) {
				mat[ i ][ i ] = s;
				mat[ i ][ mat.dimension() - 1 ] = -m[ i ] * s;
			}
		}

	template<int dim, typename _T>
		inline void PointSet<dim,_T>::normalize()
		{
			PTTYPE* pt = &_pts[ 0 ];
			PTTYPE m = mean();
			_T s = 0;
			size_t n = size();


			/* remove mean */
			while( n-- ) {
				*pt -= m;
				s += pt->length();
				pt++;
			}

			/* scale */
			s = ( Math::sqrt( ( _T ) 2 ) * ( _T ) size() ) / s;
			scale( s );
		}




	template<int dim, typename _T>
		inline size_t PointSet<dim,_T>::size( ) const
		{
			return _pts.size();
		}

	template<int dim, typename _T>
		inline void PointSet<dim,_T>::resize( size_t n )
		{
			return _pts.resize( n );
		}

	template<int dim, typename _T>
		inline const _T* PointSet<dim,_T>::ptr() const
		{
			return ( _T* ) &_pts[ 0 ];
		}

	template<int dim, typename _T>
	_T	 PointSet<dim,_T>::ssd( const PointSet<dim, _T>& ptset ) const
	{
		if( size() != ptset.size() )
			throw CVTException( "PointSets differ in size!" );

		const PTTYPE* pt1 = &_pts[ 0 ];
		const PTTYPE* pt2 = &ptset._pts[ 0 ];
		PTTYPE diff;
		size_t n = size();
		_T ret = 0;

		while( n-- ) {
			diff = *pt1++ - *pt2++;
			ret += diff.lengthSqr();
		}
		return ret;
	}

	template<int dim, typename _T>
	_T	 PointSet<dim,_T>::maxSquaredDistance( const PointSet<dim, _T>& ptset ) const
	{
		if( size() != ptset.size() )
			throw CVTException( "PointSets differ in size!" );

		const PTTYPE* pt1 = &_pts[ 0 ];
		const PTTYPE* pt2 = &ptset._pts[ 0 ];
		PTTYPE diff;
		size_t n = size();
		_T ret = 0;

		while( n-- ) {
			diff = *pt1++ - *pt2++;
			ret = Math::max( ret, diff.lengthSqr() );
		}
		return ret;
	}

	template<int dim, typename _T>
		inline typename PointSet<dim,_T>::MATTYPE PointSet<dim,_T>::alignSimilarity( const PointSet<dim,_T>& ptset ) const
		{
			if( size() != ptset.size() )
				throw CVTException( "PointSets differ in size!" );
			if( size() <= dim )
				throw CVTException( "PointSets to small!" );

			Eigen::Matrix<_T,dim,dim> mat;
			MATTYPE m;
			PTTYPE mean1, mean2;
			_T s2 = 0;
			_T mdet;

			mean1 = mean();
			mean2 = ptset.mean();

			m.setIdentity();
			mat.setZero();

			size_t n = size();
			const PTTYPE* pt1 = &(*this)[ 0 ];
			const PTTYPE* pt2 = &ptset[ 0 ];
			PTTYPE c1, c2;
			while( n-- ) {
				c1 = *pt1 - mean1;
				c2 = *pt2 - mean2;
				s2 += c1.lengthSqr();

				for( int i = 0; i < dim; i++)
					for( int k = 0; k < dim; k++)
						mat( i, k ) += c2[ i ] * c1[ k ];

				pt1++;
				pt2++;
			}

			mat /= ( _T ) size();
			s2 /= ( _T ) size();

			Eigen::Matrix<_T,dim,1> s;
			s.setOnes();

			Eigen::SVD<Eigen::Matrix<_T,dim,dim> > svd( mat );
			if( Math::abs( svd.singularValues()[ dim - 1 ] ) <= Math::EPSILONF  ) {
				if( svd.matrixU().determinant() * svd.matrixV().determinant() < 0 )
					s( dim - 1 ) = -1;
			} else {
				mdet = mat.determinant();
				if( mdet < 0 )
					s( dim - 1 ) = -1;
			}


			mat = svd.matrixU() * s.asDiagonal() * svd.matrixV().transpose();
			mat *= svd.singularValues().dot( s ) / s2;

			for( int i = 0; i < dim; i++)
				for( int k = 0; k < dim; k++)
					m[ i ][ k ] = mat( i, k );

			PTTYPE t = mean2 - m * mean1;
			for( int i = 0; i < dim; i++)
				m[ i ][ dim ] = t[ i ];

			return m;
		}

	template<int dim, typename _T>
		inline Matrix3<_T> PointSet<dim,_T>::alignPerspective( const PointSet<dim,_T>& ) const
	{
		return Matrix3<_T>().setZero();
	}

	template<>
		inline Matrix3<float> PointSet<2,float>::alignPerspective( const PointSet<2,float>& ptset ) const
		{
			if( size() != ptset.size() )
				throw CVTException( "PointSets differ in size!" );
			if( size() <= 2 )
				throw CVTException( "PointSets to small!" );

			Eigen::MatrixXf A( 2 * size(), 8 );
			Eigen::VectorXf b( 2 * size() );
			Eigen::Matrix<float, 8, 1> x;
			Matrix3<float> ret;
			Matrix3<float> sim1, sim2;
			PointSet<2,float> n1 = *this;
			PointSet<2,float> n2 = ptset;

			n1.normalize( sim1 );
			n2.normalize( sim2 );
			size_t n = size();

			for( size_t i = 0; i < n; i++ ){
				A( 2 * i, 0 )	  = 0.0;
				A( 2 * i, 1 )	  = 0.0;
				A( 2 * i, 2 )	  = 0.0;
				A( 2 * i, 3 )	  = -n1[ i ][ 0 ];
				A( 2 * i, 4 )	  = -n1[ i ][ 1 ];
				A( 2 * i, 5 )	  = -1.0;
				A( 2 * i, 6 )	  = n1[ i ][ 0 ] * n2[ i ][ 1 ];
				A( 2 * i, 7 )	  = n1[ i ][ 1 ] * n2[ i ][ 1 ];

				A( 2 * i + 1, 0 ) = n1[ i ][ 0 ];
				A( 2 * i + 1, 1 ) = n1[ i ][ 1 ];
				A( 2 * i + 1, 2 ) = 1.0;
				A( 2 * i + 1, 3 ) = 0.0;
				A( 2 * i + 1, 4 ) = 0.0;
				A( 2 * i + 1, 5 ) = 0.0;
				A( 2 * i + 1, 6 ) = -n1[ i ][ 0 ] * n2[ i ][ 0 ];
				A( 2 * i + 1, 7 ) = -n1[ i ][ 1 ] * n2[ i ][ 0 ];

				b[ 2 * i ]		  = -n2[ i ][ 1 ];
				b[ 2 * i + 1 ]	  =  n2[ i ][ 0 ];
			}

			A.svd().solve( b, &x );

			ret[ 0 ][ 0 ] = x[ 0 ];	ret[ 0 ][ 1 ] = x[ 1 ]; ret[ 0 ][ 2 ] = x[ 2 ];
			ret[ 1 ][ 0 ] = x[ 3 ];	ret[ 1 ][ 1 ] = x[ 4 ]; ret[ 1 ][ 2 ] = x[ 5 ];
			ret[ 2 ][ 0 ] = x[ 6 ];	ret[ 2 ][ 1 ] = x[ 7 ]; ret[ 2 ][ 2 ] = 1.0;

			sim2.inverseSelf();
			ret = sim2 * ret * sim1;
			ret *= 1.0f / ret[ 2 ][ 2 ];
			return ret;
		}

	template<>
		inline Matrix3<double> PointSet<2,double>::alignPerspective( const PointSet<2,double>& ptset ) const
		{
			if( size() != ptset.size() )
				throw CVTException( "PointSets differ in size!" );
			if( size() <= 2 )
				throw CVTException( "PointSets to small!" );

			Eigen::MatrixXd A( 2 * size(), 8 );
			Eigen::VectorXd b( 2 * size() );
			Eigen::Matrix<double, 8, 1> x;
			Matrix3<double> ret;
			Matrix3<double> sim1, sim2;
			PointSet<2,double> n1 = *this;
			PointSet<2,double> n2 = ptset;

			n1.normalize( sim1 );
			n2.normalize( sim2 );
			size_t n = size();

			for( size_t i = 0; i < n; i++ ){
				A( 2 * i, 0 )	  = 0.0;
				A( 2 * i, 1 )	  = 0.0;
				A( 2 * i, 2 )	  = 0.0;
				A( 2 * i, 3 )	  = -n1[ i ][ 0 ];
				A( 2 * i, 4 )	  = -n1[ i ][ 1 ];
				A( 2 * i, 5 )	  = -1.0;
				A( 2 * i, 6 )	  = n1[ i ][ 0 ] * n2[ i ][ 1 ];
				A( 2 * i, 7 )	  = n1[ i ][ 1 ] * n2[ i ][ 1 ];

				A( 2 * i + 1, 0 ) = n1[ i ][ 0 ];
				A( 2 * i + 1, 1 ) = n1[ i ][ 1 ];
				A( 2 * i + 1, 2 ) = 1.0;
				A( 2 * i + 1, 3 ) = 0.0;
				A( 2 * i + 1, 4 ) = 0.0;
				A( 2 * i + 1, 5 ) = 0.0;
				A( 2 * i + 1, 6 ) = -n1[ i ][ 0 ] * n2[ i ][ 0 ];
				A( 2 * i + 1, 7 ) = -n1[ i ][ 1 ] * n2[ i ][ 0 ];

				b[ 2 * i ]		  = -n2[ i ][ 1 ];
				b[ 2 * i + 1 ]	  =  n2[ i ][ 0 ];
			}

			A.svd().solve( b, &x );

			ret[ 0 ][ 0 ] = x[ 0 ];	ret[ 0 ][ 1 ] = x[ 1 ]; ret[ 0 ][ 2 ] = x[ 2 ];
			ret[ 1 ][ 0 ] = x[ 3 ];	ret[ 1 ][ 1 ] = x[ 4 ]; ret[ 1 ][ 2 ] = x[ 5 ];
			ret[ 2 ][ 0 ] = x[ 6 ];	ret[ 2 ][ 1 ] = x[ 7 ]; ret[ 2 ][ 2 ] = 1.0;

			sim2.inverseSelf();
			ret = sim2 * ret * sim1;
			ret *= 1.0 / ret[ 2 ][ 2 ];
			return ret;
		}


	template<int dim, typename _T>
	inline std::ostream& operator<<( std::ostream& out, const PointSet<dim,_T>& ptset )
	{
		out << "PointSet" << dim << " ( " << ptset.size() << " entries ):" << std::endl;
		const typename Vector<dim,_T>::TYPE* pt = &ptset[ 0 ];
		size_t n = ptset.size();
		while( n-- )
			std::cout << "\t" << ( *pt++ ) << std::endl;
		return out;
	}

}
#endif
