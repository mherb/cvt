#ifndef CVT_PPCA_H
#define CVT_PPCA_H

#include <vector>
#include <cvt/util/Exception.h>
#include <cvt/math/Math.h>
#include <Eigen/Core>
#include <Eigen/SVD>
#include <Eigen/LU>
#include <Eigen/QR>

namespace cvt {

	/**
	  \ingroup Math
	*/
	template<typename T>
		class PPCA {
			public:
				PPCA( size_t dimension, size_t subdimension, size_t mixcomponents );
				virtual ~PPCA();

				size_t dimension() const;
				size_t subDimension() const;
				size_t mixtureComponents() const;

				void addSample( const Eigen::Matrix<T, Eigen::Dynamic, 1>& sample );
				void addSample( const T* sample );

				void setRandomMeans();
				void setMean( size_t index, const Eigen::Matrix<T, Eigen::Dynamic, 1>& value );

				virtual void preprocessSample( Eigen::Matrix<T, Eigen::Dynamic, 1>& output, const Eigen::Matrix<T, Eigen::Dynamic, 1>& mean, const Eigen::Matrix<T, Eigen::Dynamic, 1>& value );
				virtual void postprocessMean( Eigen::Matrix<T, Eigen::Dynamic, 1>& mean );

				void calculate( size_t iterations );

				void mean( size_t i, Eigen::Matrix<T, Eigen::Dynamic, 1>& m ) const;
				T weight( size_t i ) const;
				void principleComponents( size_t i, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p ) const;
				void principleComponents( size_t i, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p,  Eigen::Matrix<T, Eigen::Dynamic, 1>& svalues ) const;
				void principleComponents( size_t i, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p,  Eigen::Matrix<T, Eigen::Dynamic, 1>& svalues, T& sigma ) const;

			private:
				size_t _dimension;
				size_t _subdimension;
				size_t _mixcomponents;

				T probability( const Eigen::Matrix<T, Eigen::Dynamic, 1>& value, const Eigen::Matrix<T, Eigen::Dynamic, 1>& mean, const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& covariance, T normalization );


				std::vector<Eigen::Matrix<T, Eigen::Dynamic, 1> > _samples;
				Eigen::Matrix<T, Eigen::Dynamic, 1>* _means;
				T* _weights;
				T* _sigmas2;
				Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>* _pc;
				Eigen::Matrix<T, Eigen::Dynamic, 1>* _evalues;
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
		_evalues = new Eigen::Matrix<T, Eigen::Dynamic,1>[ _mixcomponents ];
	}

	template<typename T>
		inline PPCA<T>::~PPCA()
		{
			delete[] _means;
			delete[] _weights;
			delete[] _sigmas2;
			delete[] _pc;
			delete[] _evalues;
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
			if( sample.rows() != ( int ) _dimension )
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
				postprocessMean( _means[ i ] );
			}
		}

	template<typename T>
		inline void PPCA<T>::setMean( size_t i, const Eigen::Matrix<T, Eigen::Dynamic, 1>& value )
		{
			if( i >= _mixcomponents )
				throw CVTException( "Out of bounds" );
			if( value.rows() != ( int ) _dimension )
				throw CVTException( "Mean value dimension invalid" );
			_means[ i ] = value;
			postprocessMean( _means[ i ] );
		}

	template<typename T>
		inline void PPCA<T>::mean( size_t i, Eigen::Matrix<T, Eigen::Dynamic, 1>& m ) const
		{
			if( i >= _mixcomponents )
				throw CVTException( "Out of bounds" );
			m = _means[ i ];
		}


	template<typename T>
		inline T PPCA<T>::weight( size_t i ) const
		{
			if( i >= _mixcomponents )
				throw CVTException( "Out of bounds" );
			return _weights[ i ];
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
			svalues = _evalues[ i ];
		}

	template<typename T>
		inline void PPCA<T>::principleComponents( size_t i, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p,  Eigen::Matrix<T, Eigen::Dynamic, 1>& svalues, T& sigma2 ) const
		{
			if( i >= _mixcomponents )
				throw CVTException( "Out of bounds" );
			p = _pc[ i ];
			svalues = _evalues[ i ];
			sigma2 = _sigmas2[ i ];
		}


	template<typename T>
	inline void PPCA<T>::preprocessSample( Eigen::Matrix<T, Eigen::Dynamic, 1>& output, const Eigen::Matrix<T, Eigen::Dynamic, 1>& , const Eigen::Matrix<T, Eigen::Dynamic, 1>& value )
	{
		output = value;
	}

	template<typename T>
	inline void PPCA<T>::postprocessMean( Eigen::Matrix<T, Eigen::Dynamic, 1>& )
	{
	}

	template<typename T>
	 inline T PPCA<T>::probability( const Eigen::Matrix<T, Eigen::Dynamic, 1>& value, const Eigen::Matrix<T, Eigen::Dynamic, 1>& mean, const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& invcovariance, T normalization )
	 {
		Eigen::Matrix<T, Eigen::Dynamic, 1> d = value - mean;
		return normalization * Math::exp( - ( ( T ) 0.5 ) * d.dot( invcovariance * d )  );
	 }

	template<typename T>
		inline void PPCA<T>::calculate( size_t iterations )
		{
			T weights[ _mixcomponents ];
			T pnormalize[ _mixcomponents ];
			Eigen::Matrix<T, Eigen::Dynamic,1> newmeans[ _mixcomponents ];
			Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> newcovar[ _mixcomponents ];
			Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> C[ _mixcomponents ];
			Eigen::Matrix<T, Eigen::Dynamic,1> samples[ _mixcomponents ];

			/* reset the prior weights for the mixture components */
			/* reset the new means */
			/* reset the covariances */
			for( size_t i = 0; i < _mixcomponents; i++ ) {
				/* for the first iteration we use the squared distance */
				pnormalize[ i ] = 1;
				C[ i ].setIdentity( _dimension, _dimension );
			}


			while( iterations-- ) {

				/* reset the prior weights for the mixture components */
				/* reset the new means */
				/* reset the covariances */
				for( size_t k = 0; k < _mixcomponents; k++ ) {
					_weights[ k ] = 0;
					newmeans[ k ] =	Eigen::Matrix<T, Eigen::Dynamic,1>::Zero( _dimension );
					newcovar[ k ] =	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>::Zero( _dimension, _dimension );
				}

				/* 1. calculate the weights for each sample using the complete gaussian
				   2. add the weighted sample to the new means
				   3. add the weighted sample to the new covar matrices
				 */

				for( size_t i = 0, end = _samples.size(); i < end;i++ ) {
					T wsum = 0;
					/* step 1 */
					for( size_t k = 0; k < _mixcomponents; k++ ) {
						/* here we evaluate the gaussian probability */
						preprocessSample( samples[ k ], _means[ k ], _samples[ i ] );
						weights[ k ] = probability( samples[ k ], _means[ k ], C[ k ], pnormalize[ k ] );
						wsum += weights[ k ];
					}

					for( size_t k = 0; k < _mixcomponents; k++ ) {
						/* step 2 */
						weights[ k ] /= wsum;
						_weights[ k ] += weights[ k ];
						newmeans[ k ] += weights[ k ] * samples[ k ];
						/* step 3 - without the new mean */
						newcovar[ k ] += weights[ k ] * samples[ k ] * samples[ k ].transpose();
					}

				}

				for( size_t k = 0; k < _mixcomponents; k++ ) {

					/* normalize new means and covariances */
					newmeans[ k ] /= _weights[ k ];
					newcovar[ k ] /= _weights[ k ];

					/* set new means */
					_means[ k ] = newmeans[ k ];

					/* substract the mean from the covariance - part of step 3 */
					newcovar[ k ] -= _means[ k ] * _means[ k ].transpose();

					if( iterations )
						postprocessMean( _means[ k ] );

					/* normalize weights */
					_weights[ k ] /= _samples.size();
					std::cout << k << " : " << _weights[ k ] << std::endl;

					/* decompose each covariance matrix to get the eigenvectors and -values ...	 */
					Eigen::JacobiSVD<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> > svd( newcovar[ k ], Eigen::ComputeFullU | Eigen::ComputeFullV );

					/* update the noise using the singular values outside the subspace */
					_sigmas2[ k ] = svd.singularValues().block( _subdimension, 0, _dimension - _subdimension , 1 ).sum() / ( T ) ( _dimension - _subdimension );
					/* get the singular values in the subspace */
					_evalues[ k ] = svd.singularValues().block( 0, 0, _subdimension, 1 );
					/* get the eigenvectors in the subspace */
					_pc[ k ] = svd.matrixU().block( 0, 0, _dimension, _subdimension );
					Eigen::Matrix<T, Eigen::Dynamic, 1> tmp =  _evalues[ k ];
					tmp.array() -= _sigmas2[ k ];
					tmp = tmp.array().sqrt();
					_pc[ k ] *= tmp.asDiagonal();

					/* calculate the complete covariance matrix in the subspace and the weights for normalization */
					C[ k ] = _pc[ k ] * _pc[ k ].transpose();
					C[ k ].diagonal().array() += _sigmas2[ k ];

					pnormalize[ k ] = _weights[ k ] / ( Math::pow( ( T ) 2.0 * ( T ) Math::PI, _dimension * ( T ) 0.5  ) * Math::sqrt( C[ k ].determinant() ) );
					C[ k ] = C[ k ].inverse();
				}

			}
		}
}

#endif
