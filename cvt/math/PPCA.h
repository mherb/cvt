#ifndef CVT_PPCA_H
#define CVT_PPCA_H

#include <vector>
#include <cvt/util/Exception.h>

namespace cvt {
	template<typename T>
		class PPCA {
			public:
				PPCA( size_t dimension, size_t subdimension, size_t mixcomponents );
				~PPCA();

				size_t dimension() const;
				size_t subDimension() const;
				size_t mixtureComponents() const;

				void addSample( const Eigen::Matrix<T, Eigen::Dynamic, 1>& sample );
				void addSample( const T* sample );

				void setRandomMeans();
				void setMean( size_t index, const Eigen::Matrix<T, Eigen::Dynamic, 1>& value );

				void calculate( size_t iterations );

				void mean( size_t i, Eigen::Matrix<T, Eigen::Dynamic, 1>& m ) const;
				void principleComponents( size_t i, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p ) const;
				void principleComponents( size_t i, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p,  Eigen::Matrix<T, Eigen::Dynamic, 1>& svalues ) const;
				void principleComponents( size_t i, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p,  Eigen::Matrix<T, Eigen::Dynamic, 1>& svalues, T& sigma ) const;

			private:
				size_t _dimension;
				size_t _subdimension;
				size_t _mixcomponents;

				std::vector<Eigen::Matrix<T, Eigen::Dynamic, 1> > _samples;
				Eigen::Matrix<T, Eigen::Dynamic, 1>* _means;
				T* _weights;
				T* _sigmas2;
				Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>* _pc;
				Eigen::Matrix<T, Eigen::Dynamic, 1>* _svalues;
		};

	template<typename T>
		inline PPCA<T>::PPCA( size_t dimension, size_t subDimension, size_t mixtureComponents ) : _dimension( dimension ), _subdimension( subDimension ), _mixcomponents( mixtureComponents )
	{
		if( subDimension > dimension )
			throw CVTException( "Subdimension has to be less than dimension");
		_means = new Eigen::Matrix<T, Eigen::Dynamic,1>[ _mixcomponents ];
		_weights = new T[ _mixcomponents ];
		_sigmas2 = new T[ _mixcomponents ];
		_pc = new Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>[ _mixcomponents ];
		_svalues = new Eigen::Matrix<T, Eigen::Dynamic,1>[ _mixcomponents ];
	}

	template<typename T>
		inline PPCA<T>::~PPCA()
		{
			delete[] _means;
			delete[] _weights;
			delete[] _sigmas2;
			delete[] _pc;
			delete[] _svalues;
		}

	template<typename T>
		inline size_t PPCA<T>::dimension() const
		{
			return _dimension;
		}

	template<typename T>
		inline size_t PPCA<T>::subDimension() const
		{
			return _subdimension;
		}

	template<typename T>
		inline size_t PPCA<T>::mixtureComponents() const
		{
			return _mixcomponents;
		}

	template<typename T>
		inline void PPCA<T>::addSample( const Eigen::Matrix<T, Eigen::Dynamic, 1>& sample )
		{
			if( sample.rows() != _dimension )
				return;
			_samples.push_back( sample );
		}

	template<typename T>
		inline void PPCA<T>::addSample( const T* data )
		{
			Eigen::Map< Eigen::Matrix<T, Eigen::Dynamic, 1> > sample( data, _dimension );
			_samples.push_back( sample );
		}


	template<typename T>
		inline void PPCA<T>::setRandomMeans()
		{
			for( size_t i = 0; i < _mixcomponents; i++ ) {
				_means[ i ] = _samples[ Math::rand( 0, _samples.size() - 1 ) ];
			}
		}

	template<typename T>
		inline void PPCA<T>::setMean( size_t i, const Eigen::Matrix<T, Eigen::Dynamic, 1>& value )
		{
			if( i >= _mixcomponents )
				throw CVTException( "Out of bounds" );
			if( value.rows() != _dimension )
				throw CVTException( "Mean value dimension invalid" );
			_means[ i ] = value;
		}

	template<typename T>
		inline void PPCA<T>::mean( size_t i, Eigen::Matrix<T, Eigen::Dynamic, 1>& m ) const
		{
			if( i >= _mixcomponents )
				throw CVTException( "Out of bounds" );
			m = _means[ i ];
		}

	template<typename T>
		inline void PPCA<T>::principleComponents( size_t i, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p ) const
		{
			if( i >= _mixcomponents )
				throw CVTException( "Out of bounds" );
			p = _pc[ i ];
		}

	template<typename T>
		inline void PPCA<T>::principleComponents( size_t i, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p,  Eigen::Matrix<T, Eigen::Dynamic, 1>& svalues ) const
		{
			if( i >= _mixcomponents )
				throw CVTException( "Out of bounds" );
			p = _pc[ i ];
			svalues = _svalues[ i ].block( 0, 0, _subdimension, 1 );
		}

	template<typename T>
		inline void PPCA<T>::principleComponents( size_t i, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p,  Eigen::Matrix<T, Eigen::Dynamic, 1>& svalues, T& sigma2 ) const
		{
			if( i >= _mixcomponents )
				throw CVTException( "Out of bounds" );
			p = _pc[ i ];
			svalues = _svalues[ i ].block( 0, 0, _subdimension, 1 );
			sigma2 = _sigmas2[ i ];
		}

	template<typename T>
		inline void PPCA<T>::calculate( size_t iterations )
		{
			T weights[ _mixcomponents ];
			T pnormalize[ _mixcomponents ];
			Eigen::Matrix<T, Eigen::Dynamic,1> newmeans[ _mixcomponents ];
			Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> newcovar[ _mixcomponents ];
			Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> C[ _mixcomponents ];

			/* reset the prior weights for the mixture components */
			/* reset the new means */
			/* reset the covariances */
			for( size_t i = 0; i < _mixcomponents; i++ ) {
				_weights[ i ] = 0;
				newmeans[ i ] =	Eigen::Matrix<T, Eigen::Dynamic,1>::Zero( _dimension );
				newcovar[ i ] =	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>::Zero( _dimension, _dimension );
			}

			/* 1. calculate the weights for each sample
			   2. add the weighted sample to the new means
			   3. add the weighted sample to the new covar matrices
			 */

			for( size_t i = 0, end = _samples.size(); i < end;i++ ) {
				T wsum = 0;
				/* step 1 */
				for( size_t k = 0; k < _mixcomponents; k++ ) {
					/* here we use the euclidean distance */
					Eigen::Matrix<T, Eigen::Dynamic,1> d = _samples[ i ] - _means[ k ];
					weights[ k ] = Math::sqrt( d.dot( d ) );
					wsum += weights[ k ];
				}
				for( size_t k = 0; k < _mixcomponents; k++ ) {
					/* step 2 */
					weights[ k ] /= wsum;
					_weights[ k ] += weights[ k ];
					newmeans[ k ] += weights[ k ] * _samples[ i ];
					/* step 3 - without the new mean */
					newcovar[ k ] += weights[ k ] * _samples[ i ] * _samples[ i ].transpose();
				}

			}

			for( size_t k = 0; k < _mixcomponents; k++ ) {
				/* normalize new means and covariances */
				newmeans[ k ] /= _weights[ k ];
				newcovar[ k ] /= _weights[ k ];

				/* substract new mean from the covariance - part of step 3 */
				newcovar[ k ] -= newmeans[ k ] * newmeans[ k ].transpose();

				/* normalize weights */
				_weights[ k ] /= _samples.size();

				/* decompose each covariance matrix to get the eigenvectors and -values ...	 */
				Eigen::SVD<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> > svd( newcovar[ k ] );
				/* update the noise using the singular values outside the subspace */
				_sigmas2[ k ] = svd.singularValues().block( _subdimension, 0, _dimension - _subdimension , 1 ).sum() / ( T ) ( _dimension - _subdimension );
				/* get the singular values in the subspace */
				_svalues[ k ] = svd.singularValues().block( 0, 0, _subdimension, 1 );
				/* get the eigenvectors in the subspace */
				_pc[ k ] = svd.matrixU().block( 0, 0, _dimension, _subdimension );

				/* calculate the complete covariance matrix in the subspace and the weights for normalization */
				C[ k ] = _pc[ k ] * _pc[ k ].transpose();
				C[ k ].diagonal().cwise() += _sigmas2[ k ];

				pnormalize[ k ] = _weights[ k ] / ( Math::pow( ( T ) 2.0 * ( T ) Math::PI, _dimension * ( T ) 0.5  ) * Math::sqrt( C[ k ].determinant() ) );
				C[ k ] = C[ k ].inverse();

					std::cout << k << " : " << _weights[ k ] << std::endl;
					std::cout << k << " : " << pnormalize[ k ] << std::endl;
			}


			while( iterations-- ) {

				/* reset the prior weights for the mixture components */
				/* reset the new means */
				/* reset the covariances */
				for( size_t i = 0; i < _mixcomponents; i++ ) {
					_weights[ i ] = 0;
					newmeans[ i ] =	Eigen::Matrix<T, Eigen::Dynamic,1>::Zero( _dimension );
					newcovar[ i ] =	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>::Zero( _dimension, _dimension );
				}

				/* 1. calculate the weights for each sample using the complete gaussian
				   2. add the weighted sample to the new means
				   3. add the weighted sample to the new covar matrices
				 */

				for( size_t i = 0, end = _samples.size(); i < end;i++ ) {
					T wsum = 0;
					/* step 1 */
					for( size_t k = 0; k < _mixcomponents; k++ ) {
						/* here we use gaussian probability */
						Eigen::Matrix<T, Eigen::Dynamic,1> d = _samples[ i ] - _means[ k ];
						weights[ k ] = pnormalize[ k ] * Math::exp( -( ( T ) 0.5 ) * d.dot( C[ k ] * d ) );
						wsum += weights[ k ];
					}

					for( size_t k = 0; k < _mixcomponents; k++ ) {
						/* step 2 */
						weights[ k ] /= wsum;
						_weights[ k ] += weights[ k ];
						newmeans[ k ] += weights[ k ] * _samples[ i ];
						/* step 3 - without the new mean */
						newcovar[ k ] += weights[ k ] * _samples[ i ] * _samples[ i ].transpose();
					}

				}

				for( size_t k = 0; k < _mixcomponents; k++ ) {
					/* normalize new means and covariances */
					newmeans[ k ] /= _weights[ k ];
					newcovar[ k ] /= _weights[ k ];

					/* substract new mean from the covariance - part of step 3 */
					newcovar[ k ] -= newmeans[ k ] * newmeans[ k ].transpose();

					/* normalize weights */
					_weights[ k ] /= _samples.size();
					std::cout << k << " : " << _weights[ k ] << std::endl;

					/* decompose each covariance matrix to get the eigenvectors and -values ...	 */
					Eigen::SVD<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> > svd( newcovar[ k ] );
					/* update the noise using the singular values outside the subspace */
					_sigmas2[ k ] = svd.singularValues().block( _subdimension, 0, _dimension - _subdimension , 1 ).sum() / ( T ) ( _dimension - _subdimension );
					/* get the singular values in the subspace */
					_svalues[ k ] = svd.singularValues().block( 0, 0, _subdimension, 1 );
					/* get the eigenvectors in the subspace */
					_pc[ k ] = svd.matrixU().block( 0, 0, _dimension, _subdimension );

					/* calculate the complete covariance matrix in the subspace and the weights for normalization */
					C[ k ] = _pc[ k ] * _pc[ k ].transpose();
					C[ k ].diagonal().cwise() += _sigmas2[ k ];
					pnormalize[ k ] = _weights[ k ] / ( Math::pow( ( ( T ) 2.0 ) * Math::PI * _sigmas2[ k ], ( T )_dimension / ( T ) 2.0 ) * Math::sqrt( C[ k ].determinant() ) );
					C[ k ] = C[ k ].inverse();
				}

				std::cout << "\n\n" << std::endl;
			}
		}
}

#endif
