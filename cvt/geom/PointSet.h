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
				void add( const PTTYPE& pt );
				void clear();
				PTTYPE& operator[]( int i );
				const PTTYPE& operator[]( int i ) const;
				PTTYPE mean() const;
				size_t size() const;
				void translate( const PTTYPE& t );
				void scale( _T t );
				void transform( const MATTYPENT& mat );
				void transform( const MATTYPE& mat );
				void normalize( MATTYPE& mat );
				MATTYPE alignSimilarity( const PointSet<dim,_T>& ptset ) const;

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
			_T inv = 1 / ( _T ) n;

			mean.zero();
			while( n-- )
				mean += *pt++;

			mean *= inv;
			return mean;
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
			PTTYPE dsqr, cur;
			PTTYPE m = mean();
			size_t n = size();

			dsqr.zero();

			/* remove mean */
			while( n-- ) {
				*pt -= m;
				dsqr += pt->lengthSqr();
				pt++;
			}

			/* scale */
			_T s = 0;
			for( int i = 0; i < dsqr.dimension(); i++ )
				s += dsqr[ i ];
			s = Math::sqrt( s ) / ( _T ) size();
			s = ( Math::sqrt( ( _T ) 2 ) ) / s;
			scale( s );
			mat.identity();

			for( int i = 0; i < mat.dimension() - 1; i++ ) {
				mat[ i ][ i ] = s;
				mat[ i ][ mat.dimension() - 1 ] = -m[ i ] * s;
			}
		}


	template<int dim, typename _T>
		inline size_t PointSet<dim,_T>::size( ) const
		{
			return _pts.size();
		}

	template<int dim, typename _T>
		inline typename PointSet<dim,_T>::MATTYPE PointSet<dim,_T>::alignSimilarity( const PointSet<dim,_T>& ptset ) const
		{
			if( size() != ptset.size() )
				throw CVTException( "PointSets differ in size!" );

			MATTYPE m1, m2, m;
			Eigen::Matrix<_T,dim,dim> mat;

			PointSet<dim,_T> n1( *this );
			PointSet<dim,_T> n2( ptset );

			n1.normalize( m1 );
			n2.normalize( m2 );

			m.identity();
			mat.setZero();

			size_t n = ptset.size();
			PTTYPE* pt1 = &n1[ 0 ];
			PTTYPE* pt2 = &n2[ 0 ];
			while( n-- ) {
				mat( 0, 0 ) += ( *pt1 )[ 0 ] * ( *pt2 )[ 0 ];
				mat( 0, 1 ) += ( *pt1 )[ 0 ] * ( *pt2 )[ 1 ];
				mat( 0, 2 ) += ( *pt1 )[ 0 ] * ( *pt2 )[ 2 ];
				mat( 1, 0 ) += ( *pt1 )[ 1 ] * ( *pt2 )[ 0 ];
				mat( 1, 1 ) += ( *pt1 )[ 1 ] * ( *pt2 )[ 1 ];
				mat( 1, 2 ) += ( *pt1 )[ 1 ] * ( *pt2 )[ 2 ];
				mat( 2, 0 ) += ( *pt1 )[ 2 ] * ( *pt2 )[ 0 ];
				mat( 2, 1 ) += ( *pt1 )[ 2 ] * ( *pt2 )[ 1 ];
				mat( 2, 2 ) += ( *pt1 )[ 2 ] * ( *pt2 )[ 2 ];
				pt1++;
				pt2++;
			}

			Eigen::SVD<Eigen::Matrix<_T,dim,dim> > svd = mat.svd();

			mat = svd.matrixU() * svd.matrixV().transpose();
			m[ 0 ][ 0 ] = mat( 0, 0 );
			m[ 0 ][ 1 ] = mat( 0, 1 );
			m[ 0 ][ 2 ] = mat( 0, 2 );
			m[ 1 ][ 0 ] = mat( 1, 0 );
			m[ 1 ][ 1 ] = mat( 1, 1 );
			m[ 1 ][ 2 ] = mat( 1, 2 );
			m[ 2 ][ 0 ] = mat( 2, 0 );
			m[ 2 ][ 1 ] = mat( 2, 1 );
			m[ 2 ][ 2 ] = mat( 2, 2 );

			m2.inverseSelf();
			return m2 * m * m1;
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
