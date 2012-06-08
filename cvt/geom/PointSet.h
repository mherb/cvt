/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_POINTSET_H
#define CVT_POINTSET_H

#include <cvt/util/SIMD.h>
#include <cvt/geom/Ellipse.h>
#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/util/Exception.h>
#include <vector>
#include <Eigen/SVD>
#include <Eigen/Eigenvalues>

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
				MATTYPE alignRigid( const PointSet<dim,_T>& ptset ) const;
				MATTYPE alignSimilarity( const PointSet<dim,_T>& ptset ) const;
				Matrix3<_T> alignPerspective( const PointSet<dim,_T>& ptset ) const;

				_T	 fitEllipse( Ellipse<_T>& ellipse ) const;

                /**
                 * Computes the essential matrix between two views from the same camera
                 * @param other the second point set
                 * @param K     intrinsic calibration matrix
                 * @return      3x3 essential Matrix from this to other
                 */
                Matrix3<_T> essentialMatrix( const PointSet<dim, _T>& other, const Matrix3<_T> & K ) const;

                /**
                 * Computes the essential matrix between two views from different cameras
                 * @param other the second point set
                 * @param K1 intrinsic matrix for this
                 * @param K2 intrinsic matrix for other
                 * @return 3x3 essential Matrix from this to other
                 */
                Matrix3<_T> essentialMatrix( const PointSet<dim, _T>& other, const Matrix3<_T> & K1, const Matrix3<_T> & K2 ) const;

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

			mean.setZero();
			while( n-- )
				mean += *pt++;

			mean /= ( _T ) size();
			return mean;
		}

	template<>
		inline PointSet<2,float>::PTTYPE PointSet<2,float>::mean() const
		{
			const PTTYPE* pt = &_pts[ 0 ];
			PTTYPE mean;
			size_t n = _pts.size();

			SIMD::instance()->sumPoints( mean, pt, n );

			mean /= ( float ) size();
			return mean;
		}

	template<>
		inline PointSet<3,float>::PTTYPE PointSet<3,float>::mean() const
		{
			const PTTYPE* pt = &_pts[ 0 ];
			PTTYPE mean;
			size_t n = _pts.size();

			SIMD::instance()->sumPoints( mean, pt, n );

			mean /= ( float ) size();
			return mean;
		}

    template<int dim, typename _T>
		inline	typename PointSet<dim,_T>::PTTYPE PointSet<dim,_T>::variance() const
		{
			const PTTYPE* pt = &_pts[ 0 ];
			size_t n = size();
			PTTYPE mu = mean();
			PTTYPE var;

			var.setZero();
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
				*pt++ += t;
		}

	template<>
		inline void PointSet<2,float>::translate( const PTTYPE& t )
		{
			PTTYPE* pt = &_pts[ 0 ];
			size_t n = _pts.size();
			SIMD::instance()->translatePoints( pt, pt, t, n );
		}

	template<>
		inline void PointSet<3,float>::translate( const PTTYPE& t )
		{
			PTTYPE* pt = &_pts[ 0 ];
			size_t n = _pts.size();
			SIMD::instance()->translatePoints( pt, pt, t, n );
		}


	template<int dim, typename _T>
		inline void PointSet<dim,_T>::scale( _T s )
		{
			PTTYPE* pt = &_pts[ 0 ];
			size_t n = _pts.size();
			while( n-- )
				*pt++ *= s;
		}

	template<>
		inline void PointSet<2,float>::scale( float s )
		{
			float* pt = ( float* ) &_pts[ 0 ];
			size_t n = _pts.size();
			SIMD::instance()->MulValue1f( pt, pt, s, n * 2 );
		}

	template<>
		inline void PointSet<3,float>::scale( float s )
		{
			float* pt = ( float* ) &_pts[ 0 ];
			size_t n = _pts.size();
			SIMD::instance()->MulValue1f( pt, pt, s, n * 3 );
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
				SIMD::instance()->transformPoints( pt, mat, pt, n );
			} else {
				SIMD::instance()->transformPointsHomogenize( pt, mat, pt, n );
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
			if( mat[ 2 ] == Vector4f( 0, 0, 0, 1.0f ) ) {
				SIMD::instance()->transformPoints( pt, mat, pt, n );
			} else {
				SIMD::instance()->transformPointsHomogenize( pt, mat, pt, n );
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

	template <int dim, typename _T>
	inline typename PointSet<dim,_T>::MATTYPE PointSet<dim,_T>::alignRigid( const PointSet<dim,_T>& ptset ) const
	{
		if( size() != ptset.size() )
			throw CVTException( "PointSets differ in size!" );
		if( size() <= dim )
			throw CVTException( "PointSets to small!" );

		Eigen::Matrix<_T,dim,dim> mat;
		MATTYPE m;
		PTTYPE mean1, mean2;
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

			for( int i = 0; i < dim; i++)
				for( int k = 0; k < dim; k++)
					mat( i, k ) += c2[ i ] * c1[ k ];

			pt1++;
			pt2++;
		}

		mat /= ( _T ) size();

		Eigen::Matrix<_T,dim,1> s;
		s.setOnes();

		Eigen::JacobiSVD<Eigen::Matrix<_T,dim,dim> > svd( mat, Eigen::ComputeFullU | Eigen::ComputeFullV );		
		if( Math::abs( svd.singularValues()[ dim - 1 ] ) <= Math::EPSILONF  ) { // TODO: why?
			if( svd.matrixU().determinant() * svd.matrixV().determinant() < 0 )
				s( dim - 1 ) = -1;
		} else {
			mdet = mat.determinant();
			if( mdet < 0 )
				s( dim - 1 ) = -1;
		}

		mat = svd.matrixU() * s.asDiagonal() * svd.matrixV().transpose();

		for( int i = 0; i < dim; i++)
			for( int k = 0; k < dim; k++)
				m[ i ][ k ] = mat( i, k );

		PTTYPE t = mean2 - m * mean1;
		for( int i = 0; i < dim; i++)
			m[ i ][ dim ] = t[ i ];

		return m;
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

			Eigen::JacobiSVD<Eigen::Matrix<_T,dim,dim> > svd( mat, Eigen::ComputeFullU | Eigen::ComputeFullV );
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

			Eigen::JacobiSVD<Eigen::MatrixXf> svd( A, Eigen::ComputeThinU | Eigen::ComputeThinV );
			x = svd.solve( b );

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

			Eigen::JacobiSVD<Eigen::MatrixXd> svd( A, Eigen::ComputeThinU | Eigen::ComputeThinV );
			x = svd.solve( b );

			ret[ 0 ][ 0 ] = x[ 0 ];	ret[ 0 ][ 1 ] = x[ 1 ]; ret[ 0 ][ 2 ] = x[ 2 ];
			ret[ 1 ][ 0 ] = x[ 3 ];	ret[ 1 ][ 1 ] = x[ 4 ]; ret[ 1 ][ 2 ] = x[ 5 ];
			ret[ 2 ][ 0 ] = x[ 6 ];	ret[ 2 ][ 1 ] = x[ 7 ]; ret[ 2 ][ 2 ] = 1.0;

			sim2.inverseSelf();
			ret = sim2 * ret * sim1;
			ret *= 1.0 / ret[ 2 ][ 2 ];
			return ret;
		}

		template<int dim, typename _T>
    	inline Matrix3<_T> PointSet<dim, _T>::essentialMatrix( const PointSet<dim, _T>&, const Matrix3<_T> & ) const
    	{
    	    Matrix3<_T> E;
    	    E.setZero();
    	    return E;
    	}

    template<>
    inline Matrix3<double> PointSet<2, double>::essentialMatrix( const PointSet<2, double>& other, const Matrix3<double> & K ) const
    {
        if( this->size() < 8 )
            throw CVTException( "essential Matrix computation needs at least 8 points" );
        if( this->size() != other.size() )
            throw CVTException( "point sets have different size!" );

        Matrix3d Kinv = K;
        if( !Kinv.inverseSelf() ){
            throw CVTException( "Could not invert Intrinsic Matrix!" );
		}

        // create copies of the point set
        PointSet2d set0( *this ), set1( other );

        // transform the pointsets with Kinv:
        set0.transform( Kinv );
        set1.transform( Kinv );


        Matrix3d sim0, sim1;
        set0.normalize( sim0 );
        set1.normalize( sim1 );

        // each point corresp is one point
        Eigen::Matrix<double, 9, 9> A( Eigen::Matrix<double, 9, 9>::Zero() );
        Eigen::Matrix<double, 9, 1> tmp;

        for( size_t i = 0; i < size(); i++ ){
            const Vector2d & p0 = set0[ i ];
            const Vector2d & p1 = set1[ i ];
            tmp[ 0 ] = p0.x * p1.x;
            tmp[ 1 ] = p0.y * p1.x;
            tmp[ 2 ] = p1.x;
            tmp[ 3 ] = p0.x * p1.y;
            tmp[ 4 ] = p0.y * p1.y;
            tmp[ 5 ] = p1.y;
            tmp[ 6 ] = p0.x;
            tmp[ 7 ] = p0.y;
            tmp[ 8 ] = 1;

            for( size_t r = 0; r < 9; r++ ){
                for( size_t c = r; c < 9; c++ ){
                    A( r, c ) += tmp[ r ] * tmp[ c ];
                }
            }
        }

        for( size_t r = 1; r < 9; r++ ){
            for( size_t c = 0; c < r; c++ ){
                A( r, c ) = A( c, r );
            }
        }

        Eigen::JacobiSVD<Eigen::Matrix<double, 9, 9> > svd( A, Eigen::ComputeFullU | Eigen::ComputeFullV );

        const Eigen::Matrix<double, 9, 1> & eVec = svd.matrixV().col( 8 );

        Eigen::Matrix<double, 3, 3> eTmp;
        eTmp( 0, 0 ) = eVec[ 0 ];
        eTmp( 0, 1 ) = eVec[ 1 ];
        eTmp( 0, 2 ) = eVec[ 2 ];
        eTmp( 1, 0 ) = eVec[ 3 ];
        eTmp( 1, 1 ) = eVec[ 4 ];
        eTmp( 1, 2 ) = eVec[ 5 ];
        eTmp( 2, 0 ) = eVec[ 6 ];
        eTmp( 2, 1 ) = eVec[ 7 ];
        eTmp( 2, 2 ) = eVec[ 8 ];

        // enforce singularity constraint:
        Eigen::JacobiSVD<Eigen::Matrix<double, 3, 3> > esvd( eTmp, Eigen::ComputeFullU | Eigen::ComputeFullV );
        Eigen::Vector3d singVals = esvd.singularValues();
        singVals[ 2 ] = 0;
        eTmp = esvd.matrixU() * singVals.asDiagonal() * esvd.matrixV().transpose();

        Matrix3d E;
        E[ 0 ][ 0 ] = eTmp( 0, 0 );
        E[ 0 ][ 1 ] = eTmp( 0, 1 );
        E[ 0 ][ 2 ] = eTmp( 0, 2 );

        E[ 1 ][ 0 ] = eTmp( 1, 0 );
        E[ 1 ][ 1 ] = eTmp( 1, 1 );
        E[ 1 ][ 2 ] = eTmp( 1, 2 );

        E[ 2 ][ 0 ] = eTmp( 2, 0 );
        E[ 2 ][ 1 ] = eTmp( 2, 1 );
        E[ 2 ][ 2 ] = eTmp( 2, 2 );

        Matrix3d Et = sim1.transpose() * E * sim0;

        return Et;
    }

    template<>
    inline Matrix3<float> PointSet<2, float>::essentialMatrix( const PointSet<2, float>& other, const Matrix3<float> & K ) const
    {
        if( this->size() < 8 )
            throw CVTException( "essential Matrix computation needs at least 8 points" );
        if( this->size() != other.size() )
            throw CVTException( "point sets have different size!" );

		Matrix3f Kinv = K;
        if( !Kinv.inverseSelf() ){
            throw CVTException( "Could not invert Intrinsic Matrix!" );
        }

        // create copies of the point set
        PointSet2f set0( *this ), set1( other );

        // transform the pointsets with Kinv:
        set0.transform( Kinv );
        set1.transform( Kinv );


        Matrix3f sim0, sim1;
        set0.normalize( sim0 );
        set1.normalize( sim1 );

        // each point corresp is one point
        Eigen::Matrix<float, 9, 9> A( Eigen::Matrix<float, 9, 9>::Zero() );
        Eigen::Matrix<float, 9, 1> tmp;

        for( size_t i = 0; i < size(); i++ ){
            const Vector2f & p0 = set0[ i ];
            const Vector2f & p1 = set1[ i ];
            tmp[ 0 ] = p0.x * p1.x;
            tmp[ 1 ] = p0.y * p1.x;
            tmp[ 2 ] = p1.x;
            tmp[ 3 ] = p0.x * p1.y;
            tmp[ 4 ] = p0.y * p1.y;
            tmp[ 5 ] = p1.y;
            tmp[ 6 ] = p0.x;
            tmp[ 7 ] = p0.y;
            tmp[ 8 ] = 1;

            for( size_t r = 0; r < 9; r++ ){
                for( size_t c = r; c < 9; c++ ){
                    A( r, c ) += tmp[ r ] * tmp[ c ];
                }
            }
        }

        for( size_t r = 1; r < 9; r++ ){
            for( size_t c = 0; c < r; c++ ){
                A( r, c ) = A( c, r );
            }
        }

        Eigen::JacobiSVD<Eigen::Matrix<float, 9, 9> > svd( A, Eigen::ComputeFullU | Eigen::ComputeFullV );

        const Eigen::Matrix<float, 9, 1> & eVec = svd.matrixV().col( 8 );

        Eigen::Matrix<float, 3, 3> eTmp;
        eTmp( 0, 0 ) = eVec[ 0 ];
        eTmp( 0, 1 ) = eVec[ 1 ];
        eTmp( 0, 2 ) = eVec[ 2 ];
        eTmp( 1, 0 ) = eVec[ 3 ];
        eTmp( 1, 1 ) = eVec[ 4 ];
        eTmp( 1, 2 ) = eVec[ 5 ];
        eTmp( 2, 0 ) = eVec[ 6 ];
        eTmp( 2, 1 ) = eVec[ 7 ];
        eTmp( 2, 2 ) = eVec[ 8 ];

        // enforce singularity constraint:
        Eigen::JacobiSVD<Eigen::Matrix<float, 3, 3> > esvd( eTmp, Eigen::ComputeFullU | Eigen::ComputeFullV );
        Eigen::Vector3f singVals = esvd.singularValues();
        singVals[ 2 ] = 0;
        eTmp = esvd.matrixU() * singVals.asDiagonal() * esvd.matrixV().transpose();

        Matrix3f E;
        E[ 0 ][ 0 ] = eTmp( 0, 0 );
        E[ 0 ][ 1 ] = eTmp( 0, 1 );
        E[ 0 ][ 2 ] = eTmp( 0, 2 );

        E[ 1 ][ 0 ] = eTmp( 1, 0 );
        E[ 1 ][ 1 ] = eTmp( 1, 1 );
        E[ 1 ][ 2 ] = eTmp( 1, 2 );

        E[ 2 ][ 0 ] = eTmp( 2, 0 );
        E[ 2 ][ 1 ] = eTmp( 2, 1 );
        E[ 2 ][ 2 ] = eTmp( 2, 2 );

        Matrix3f Et = sim1.transpose() * E * sim0;

        return Et;
    }

    template<int dim, typename _T>
    inline Matrix3<_T> PointSet<dim, _T>::essentialMatrix( const PointSet<dim, _T>&, const Matrix3<_T> &, const Matrix3<_T> & ) const
    {
        Matrix3<_T> E;
        E.setZero();
        return E;
    }

    template<>
    inline Matrix3<double> PointSet<2, double>::essentialMatrix( const PointSet<2, double>& other, const Matrix3<double> & K1, const Matrix3<double> & K2 ) const
    {
        if( this->size() < 8 )
            throw CVTException( "essential Matrix computation needs at least 8 points" );

        if( this->size() != other.size() )
            throw CVTException( "point sets have different size!" );

        Matrix3d K1inv = K1;
        Matrix3d K2inv = K2;
        if( !K1inv.inverseSelf() ){
            throw CVTException( "Could not invert Intrinsic Matrix for pointset 0!" );
        }

        if( !K2inv.inverseSelf() ){
            throw CVTException( "Could not invert Intrinsic Matrix for pointset 1!" );
        }

        // create copies of the point set
        PointSet2d set0( *this ), set1( other );

        // transform the pointsets with Kinv:
        set0.transform( K1inv );
        set1.transform( K2inv );


        Matrix3d sim0, sim1;
        set0.normalize( sim0 );
        set1.normalize( sim1 );

        // each point corresp is one point
        Eigen::Matrix<double, 9, 9> A( Eigen::Matrix<double, 9, 9>::Zero() );
        Eigen::Matrix<double, 9, 1> tmp;

        for( size_t i = 0; i < size(); i++ ){
            const Vector2d & p0 = set0[ i ];
            const Vector2d & p1 = set1[ i ];
            tmp[ 0 ] = p0.x * p1.x;
            tmp[ 1 ] = p0.y * p1.x;
            tmp[ 2 ] = p1.x;
            tmp[ 3 ] = p0.x * p1.y;
            tmp[ 4 ] = p0.y * p1.y;
            tmp[ 5 ] = p1.y;
            tmp[ 6 ] = p0.x;
            tmp[ 7 ] = p0.y;
            tmp[ 8 ] = 1;

            for( size_t r = 0; r < 9; r++ ){
                for( size_t c = r; c < 9; c++ ){
                    A( r, c ) += tmp[ r ] * tmp[ c ];
                }
            }
        }

        for( size_t r = 1; r < 9; r++ ){
            for( size_t c = 0; c < r; c++ ){
                A( r, c ) = A( c, r );
            }
        }

        Eigen::JacobiSVD<Eigen::Matrix<double, 9, 9> > svd( A, Eigen::ComputeFullU | Eigen::ComputeFullV );

        const Eigen::Matrix<double, 9, 1> & eVec = svd.matrixV().col( 8 );

        Eigen::Matrix<double, 3, 3> eTmp;
        eTmp( 0, 0 ) = eVec[ 0 ];
        eTmp( 0, 1 ) = eVec[ 1 ];
        eTmp( 0, 2 ) = eVec[ 2 ];
        eTmp( 1, 0 ) = eVec[ 3 ];
        eTmp( 1, 1 ) = eVec[ 4 ];
        eTmp( 1, 2 ) = eVec[ 5 ];
        eTmp( 2, 0 ) = eVec[ 6 ];
        eTmp( 2, 1 ) = eVec[ 7 ];
        eTmp( 2, 2 ) = eVec[ 8 ];

        // enforce singularity constraint:
        Eigen::JacobiSVD<Eigen::Matrix<double, 3, 3> > esvd( eTmp, Eigen::ComputeFullU | Eigen::ComputeFullV );
        Eigen::Vector3d singVals = esvd.singularValues();
        singVals[ 2 ] = 0;
        eTmp = esvd.matrixU() * singVals.asDiagonal() * esvd.matrixV().transpose();

        Matrix3d E;
        E[ 0 ][ 0 ] = eTmp( 0, 0 );
        E[ 0 ][ 1 ] = eTmp( 0, 1 );
        E[ 0 ][ 2 ] = eTmp( 0, 2 );

        E[ 1 ][ 0 ] = eTmp( 1, 0 );
        E[ 1 ][ 1 ] = eTmp( 1, 1 );
        E[ 1 ][ 2 ] = eTmp( 1, 2 );

        E[ 2 ][ 0 ] = eTmp( 2, 0 );
        E[ 2 ][ 1 ] = eTmp( 2, 1 );
        E[ 2 ][ 2 ] = eTmp( 2, 2 );

        Matrix3d Et = sim1.transpose() * E * sim0;

        return Et;
    }

    template<>
    inline Matrix3<float> PointSet<2, float>::essentialMatrix( const PointSet<2, float>& other, const Matrix3<float> & K1, const Matrix3<float> & K2 ) const
    {
        if( this->size() < 8 )
            throw CVTException( "essential Matrix computation needs at least 8 points" );

        if( this->size() != other.size() )
            throw CVTException( "point sets have different size!" );

        Matrix3f K1inv = K1;
        Matrix3f K2inv = K2;
        if( !K1inv.inverseSelf() ){
            throw CVTException( "Could not invert Intrinsic Matrix for pointset 0!" );
        }

        if( !K2inv.inverseSelf() ){
            throw CVTException( "Could not invert Intrinsic Matrix for pointset 1!" );
        }

        // create copies of the point set
        PointSet2f set0( *this ), set1( other );

        // transform the pointsets with Kinv:
        set0.transform( K1inv );
        set1.transform( K2inv );


        Matrix3f sim0, sim1;
        set0.normalize( sim0 );
        set1.normalize( sim1 );

        // each point corresp is one point
        Eigen::Matrix<float, 9, 9> A( Eigen::Matrix<float, 9, 9>::Zero() );
        Eigen::Matrix<float, 9, 1> tmp;

        for( size_t i = 0; i < size(); i++ ){
            const Vector2f & p0 = set0[ i ];
            const Vector2f & p1 = set1[ i ];
            tmp[ 0 ] = p0.x * p1.x;
            tmp[ 1 ] = p0.y * p1.x;
            tmp[ 2 ] = p1.x;
            tmp[ 3 ] = p0.x * p1.y;
            tmp[ 4 ] = p0.y * p1.y;
            tmp[ 5 ] = p1.y;
            tmp[ 6 ] = p0.x;
            tmp[ 7 ] = p0.y;
            tmp[ 8 ] = 1;

            for( size_t r = 0; r < 9; r++ ){
                for( size_t c = r; c < 9; c++ ){
                    A( r, c ) += tmp[ r ] * tmp[ c ];
                }
            }
        }

        for( size_t r = 1; r < 9; r++ ){
            for( size_t c = 0; c < r; c++ ){
                A( r, c ) = A( c, r );
            }
        }

        Eigen::JacobiSVD<Eigen::Matrix<float, 9, 9> > svd( A, Eigen::ComputeFullU | Eigen::ComputeFullV );

        const Eigen::Matrix<float, 9, 1> & eVec = svd.matrixV().col( 8 );

        Eigen::Matrix<float, 3, 3> eTmp;
        eTmp( 0, 0 ) = eVec[ 0 ];
        eTmp( 0, 1 ) = eVec[ 1 ];
        eTmp( 0, 2 ) = eVec[ 2 ];
        eTmp( 1, 0 ) = eVec[ 3 ];
        eTmp( 1, 1 ) = eVec[ 4 ];
        eTmp( 1, 2 ) = eVec[ 5 ];
        eTmp( 2, 0 ) = eVec[ 6 ];
        eTmp( 2, 1 ) = eVec[ 7 ];
        eTmp( 2, 2 ) = eVec[ 8 ];

        // enforce singularity constraint:
        Eigen::JacobiSVD<Eigen::Matrix<float, 3, 3> > esvd( eTmp, Eigen::ComputeFullU | Eigen::ComputeFullV );
        Eigen::Vector3f singVals = esvd.singularValues();
        singVals[ 2 ] = 0;
        eTmp = esvd.matrixU() * singVals.asDiagonal() * esvd.matrixV().transpose();

        Matrix3f E;
        E[ 0 ][ 0 ] = eTmp( 0, 0 );
        E[ 0 ][ 1 ] = eTmp( 0, 1 );
        E[ 0 ][ 2 ] = eTmp( 0, 2 );

        E[ 1 ][ 0 ] = eTmp( 1, 0 );
        E[ 1 ][ 1 ] = eTmp( 1, 1 );
        E[ 1 ][ 2 ] = eTmp( 1, 2 );

        E[ 2 ][ 0 ] = eTmp( 2, 0 );
        E[ 2 ][ 1 ] = eTmp( 2, 1 );
        E[ 2 ][ 2 ] = eTmp( 2, 2 );

        Matrix3f Et = sim1.transpose() * E * sim0;

        return Et;
    }


	template<int dim, typename _T>
	_T	 PointSet<dim,_T>::fitEllipse( Ellipse<_T>& ellipse ) const
	{
		if( dim != 2 )
			return INFINITY;

		typedef Eigen::Matrix<_T, 6, 6> EigenMatrix6;
		typedef Eigen::Matrix<_T, 3, 3> EigenMatrix3;
		typedef Eigen::Matrix<_T, 3, 1> EigenVector3;
		typedef Eigen::Matrix<_T, 6, 1> EigenVector6;

		EigenMatrix6 scatter;
		_T x2, y2, xy, x, y;
		_T mx, my, sx, sy;

		MATTYPE sim;

		// normalize the points
		PointSet<dim,_T> points = *this;
		points.normalize( sim );

		sim.inverseSelf();
		sx = sim[ 0 ][ 0 ];
		sy = sim[ 1 ][ 1 ];
		mx = sim[ 0 ][ 2 ];
		my = sim[ 1 ][ 2 ];

		// compute upper scatter matrix
		scatter.setZero();

		size_t n = points.size();
		const PTTYPE* pt = &points[ 0 ];

		while( n-- ) {
			x	= pt->x;
			y	= pt->y;
			pt++;
			x2	= x * x;
			y2	= y * y;
			xy	= x * y;

			scatter( 0 , 0 ) += x2 * x2;
			scatter( 0 , 1 ) += x2 * xy;
			scatter( 0 , 2 ) += x2 * y2;
			scatter( 0 , 3 ) += x2 * x;
			scatter( 0 , 4 ) += x2 * y;
			scatter( 0 , 5 ) += x2;

			scatter( 1 , 1 ) += xy * xy;
			scatter( 1 , 2 ) += xy * y2;
			scatter( 1 , 3 ) += xy * x;
			scatter( 1 , 4 ) += xy * y;
			scatter( 1 , 5 ) += xy;

			scatter( 2 , 2 ) += y2 * y2;
			scatter( 2 , 3 ) += y2 * x;
			scatter( 2 , 4 ) += y2 * y;
			scatter( 2 , 5 ) += y2;

			scatter( 3 , 3 ) += x * x;
			scatter( 3 , 4 ) += x * y;
			scatter( 3 , 5 ) += x;

			scatter( 4 , 4 ) += y * y;
			scatter( 4 , 5 ) += y;

			scatter( 5 , 5 ) += ( _T ) 1;
		}

		// make matrix symmetric

		scatter( 1 , 0 ) = scatter( 0 , 1 );
		scatter( 2 , 0 ) = scatter( 0 , 2 );
		scatter( 3 , 0 ) = scatter( 0 , 3 );
		scatter( 4 , 0 ) = scatter( 0 , 4 );
		scatter( 5 , 0 ) = scatter( 0 , 5 );

		scatter( 2 , 1 ) = scatter( 1 , 2 );
		scatter( 3 , 1 ) = scatter( 1 , 3 );
		scatter( 4 , 1 ) = scatter( 1 , 4 );
		scatter( 5 , 1 ) = scatter( 1 , 5 );

		scatter( 3 , 2 ) = scatter( 2 , 3 );
		scatter( 4 , 2 ) = scatter( 2 , 4 );
		scatter( 5 , 2 ) = scatter( 2 , 5 );

		scatter( 4 , 3 ) = scatter( 3 , 4 );
		scatter( 5 , 3 ) = scatter( 3 , 5 );

		scatter( 5 , 4 ) = scatter( 4 , 5 );

		// build up constraint matrix
		EigenMatrix6 C;

		C << 0.0f, 0.0f, -2.0f, 0.0f, 0.0f, 0.0f,
		  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		  -2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f;

		EigenMatrix3 tmpA;

		tmpA << scatter(0,0), scatter(0,1), scatter(0,2),
			 scatter(1,0), scatter(1,1), scatter(1,2),
			 scatter(2,0), scatter(2,1), scatter(2,2);

		EigenMatrix3 tmpB;

		tmpB << scatter(0,3), scatter(0,4), scatter(0,5),
			 scatter(1,3), scatter(1,4), scatter(1,5),
			 scatter(2,3), scatter(2,4), scatter(2,5);


		EigenMatrix3 tmpC;

		tmpC << scatter(3,3), scatter(3,4), scatter(3,5),
			 scatter(4,3), scatter(4,4), scatter(4,5),
			 scatter(5,3), scatter(5,4), scatter(5,5);

		EigenMatrix3 tmpD;

		tmpD << C(0,0), C(0,1), C(0,2),
			 C(1,0), C(1,1), C(1,2),
			 C(2,0), C(2,1), C(2,2);

		EigenMatrix3 tmpE = tmpC.inverse() * tmpB.transpose();
		EigenMatrix3 A = tmpD.inverse() * ( tmpA - tmpB * tmpE );

		Eigen::SelfAdjointEigenSolver<EigenMatrix3> solver( A );
		typename Eigen::SelfAdjointEigenSolver<EigenMatrix3>::MatrixType eigenVectors = solver.eigenvectors();

		EigenVector3 top( eigenVectors( 0, 0 ), eigenVectors( 1, 0 ), eigenVectors( 2, 0 ) );
		EigenVector3 bottom = -tmpE * top;

		EigenVector6 parNorm;

		parNorm << top(0), top(1), top(2),
				bottom(0), bottom(1), bottom(2);

		// unnormalize system parameters
		EigenVector6 par;

		par(0) = parNorm(0)*sy*sy;
		par(1) = parNorm(1)*sx*sy;
		par(2) = parNorm(2)*sx*sx;
		par(3) = -2.0f*parNorm(0)*sy*sy*mx - parNorm(1)*sx*sy*my + parNorm(3)*sx*sy*sy;
		par(4) = -parNorm(1)*sx*sy*mx - 2.0f*top(2)*sx*sx*my + parNorm(4)*sx*sx*sy;
		par(5) = parNorm(0)*sy*sy*mx*mx + parNorm(1)*sx*sy*mx*my + parNorm(2)*sx*sx*my*my
			- parNorm(3)*sx*sy*sy*mx - parNorm(4)*sx*sx*sy*my
			+ parNorm(5)*sx*sx*sy*sy;

		// compute ellipse parameters

		_T thetarad = Math::atan2( double(par(1)), double(par(0) - par(2)) ) * ( _T ) 0.5;
		_T cost = Math::cos( thetarad );
		_T sint = Math::sin( thetarad );
		_T sin_squared = sint * sint;
		_T cos_squared = cost * cost;
		_T cos_sin = sint * cost;

		_T Ao =  par(5);
		_T Au =  par(3) * cost + par(4) * sint;
		_T Av = -par(3) * sint + par(4) * cost;
		_T Auu = par(0) * cos_squared + par(2) * sin_squared + par(1) * cos_sin;
		_T Avv = par(0) * sin_squared + par(2) * cos_squared - par(1) * cos_sin;

		_T tuCentre = - Au/(2*Auu);
		_T tvCentre = - Av/(2*Avv);
		_T wCentre = Ao - Auu*tuCentre*tuCentre - Avv*tvCentre*tvCentre;

		ellipse.center().x = tuCentre * cost - tvCentre * sint;
		ellipse.center().y = tuCentre * sint + tvCentre * cost;

		_T Ru = -wCentre/Auu;
		_T Rv = -wCentre/Avv;

		ellipse.semiMajor() = Math::sqrt( Math::abs( Ru ) );
		ellipse.semiMinor() = Math::sqrt( Math::abs( Rv ) );

		ellipse.orientation() = thetarad;

//		params.AxesDirection[0].x = cost;
//		params.AxesDirection[0].y = sint;
//
//		params.AxesDirection[1].x = -sint;
//		params.AxesDirection[1].y = cost;

		// return fit error
		_T ret = parNorm.transpose() * scatter * parNorm;
		return ret / ( _T ) points.size();
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
