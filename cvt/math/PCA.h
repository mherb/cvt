#ifndef CVT_PCA_H
#define CVT_PCA_H

#include <Eigen/Core>
#include <Eigen/SVD>

namespace cvt {

	template<typename T>
	class PCA
	{
		public:
			PCA( size_t dimension );
			~PCA();

			size_t dimension() const;

			void addSample( const Eigen::Matrix<T, Eigen::Dynamic, 1>& sample );
			void addSample( const T* sample );
//			void addSample( T* sample, size_t width, size_t stride );

			void mean( Eigen::Matrix<T, Eigen::Dynamic, 1>& m ) const;
			void principleComponents(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p) const;
			void principleComponents( Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p,  Eigen::Matrix<T, Eigen::Dynamic, 1>& svalues ) const;

		private:
			PCA();
			PCA( const PCA& pca );

			size_t _dimension;
			size_t _n;

			Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> _x;
			Eigen::Matrix<T, Eigen::Dynamic, 1>				 _mean;
	};

	template<typename T>
	inline PCA<T>::PCA( size_t dimension ) : _dimension( dimension ), _n( 0 ), _x( ( int ) dimension, ( int ) dimension ), _mean( dimension )
	{
		_mean.setZero();
		_x.setZero();
	}

	template<typename T>
	inline PCA<T>::~PCA()
	{
	}


	template<typename T>
	size_t PCA<T>::dimension() const
	{
		return _dimension;
	}

	template<typename T>
	inline void PCA<T>::addSample( const Eigen::Matrix<T, Eigen::Dynamic, 1>& sample )
	{
		if( sample.rows() != _dimension )
			return;
		_mean += sample;
		_x	  += sample * sample.transpose();
		_n++;
	}

	template<typename T>
	inline void PCA<T>::addSample( const T* data )
	{
		Eigen::Map< Eigen::Matrix<T, Eigen::Dynamic, 1> > sample( ( T* )data, _dimension );
		_mean += sample;
		_x	  += sample * sample.transpose();
		_n++;
	}


	template<typename T>
	inline void PCA<T>::mean( Eigen::Matrix<T, Eigen::Dynamic, 1>& m ) const
	{
		if( ( size_t ) m.rows() != _dimension || !_n )
			return;
		T invn = 1 / ( T ) _n;
		m = _mean * invn;
	}


	template<typename T>
	void PCA<T>::principleComponents(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p) const
	{
		if( ( size_t ) p.rows() != _dimension || ( size_t ) p.cols() != _dimension || !_n )
			return;

		T invn = 1 / ( T ) _n;
		Eigen::Matrix<T, Eigen::Dynamic, 1> m( _dimension );
		mean( m );

		p = _x * invn - m * m.transpose();
		Eigen::JacobiSVD<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> > svd( p, Eigen::ComputeThinU | Eigen::ComputeThinV );
		p = svd.matrixU();
	}

	template<typename T>
	void PCA<T>::principleComponents( Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p,  Eigen::Matrix<T, Eigen::Dynamic, 1>& svalues ) const
	{
		if( ( size_t ) p.rows() != _dimension || ( size_t ) p.cols() != _dimension || !_n )
			return;

		T invn = 1 / ( T ) _n;
		Eigen::Matrix<T, Eigen::Dynamic, 1> m( _dimension );
		mean( m );

		p = _x * invn - m * m.transpose();
		Eigen::JacobiSVD<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> > svd( p, Eigen::ComputeThinU | Eigen::ComputeThinV );
		svalues = svd.singularValues();
		p = svd.matrixU();
	}

	typedef PCA<float> PCAf;
	typedef PCA<double> PCAd;
}

#endif
