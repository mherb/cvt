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

				void calculate();

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
				Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>* _covariances;
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
		_covariances = new Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>[ _mixcomponents ];
		_pc = new Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>[ _mixcomponents ];
		_svalues = new Eigen::Matrix<T, Eigen::Dynamic,1>[ _mixcomponents ];
	}

	template<typename T>
		inline PPCA<T>::~PPCA()
		{
			delete[] _means;
			delete[] _weights;
			delete[] _sigmas2;
			delete[] _covariances;
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
				_means[ i ] = _samples[ Math::rand( 0, _samples.size() ) ];
			}
		}

	template<typename T>
		inline void PPCA::setMean( size_t index, const Eigen::Matrix<T, Eigen::Dynamic, 1>& value )
		{
			if( i >= _mixcomponents )
				throw CVTException( "Out of bounds" );
			if( value.rows() != _dimension )
				throw CVTException( "Mean value dimension invalid" );
			_means[ index ] = value;
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
		inline void PPCA<T>::calculate()
		{

			/* ... lots of stuff ... */

			/* update noise */
			for( int i = 0; i < _mixcomponents; i++ )
				_sigma2[ i ] = _svalues[ i ].block( _subdimension, 0, _dimension - _subdimension , 1 ).sum() / ( T ) ( _dimension - _subdimension );

			/* ... other stuff ... */
		}
}

#endif
