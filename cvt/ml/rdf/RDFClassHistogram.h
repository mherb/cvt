#ifndef CVT_RDCLASSHISTOGRAM_H
#define CVT_RDCLASSHISTOGRAM_H

#include <iostream>
#include <string.h>

#include <cvt/math/Math.h>


namespace cvt {
	template<size_t N>
	class RDFClassHistogram
	{
		public:
			RDFClassHistogram();
			RDFClassHistogram( const RDFClassHistogram<N>& other );
			~RDFClassHistogram();

			RDFClassHistogram<N>& operator=( const RDFClassHistogram<N>& other );
			RDFClassHistogram<N>& operator+=( const RDFClassHistogram<N>& other );

			float				  probability( size_t ) const;
			float				  entropy() const;

			void				  addSample( size_t classLabel );
			size_t				  sampleCount() const;
			void				  clear();

		private:
			size_t		 _numSamples;
			size_t		 _bin[ N ];
	};


	template<size_t N>
	inline RDFClassHistogram<N>::RDFClassHistogram() : _numSamples( 0 )
	{
		clear();
	}

	template<size_t N>
	inline RDFClassHistogram<N>::RDFClassHistogram( const RDFClassHistogram<N>& other ) : _numSamples( other._numSamples )
	{
		::memcpy( _bin, other._bin, sizeof( size_t ) * N );
	}

	template<size_t N>
	inline RDFClassHistogram<N>::~RDFClassHistogram()
	{
	}

	template<size_t N>
	inline float RDFClassHistogram<N>::probability( size_t classN ) const
	{
		return ( float ) _bin[ classN ] / ( float ) _numSamples;
	}

	template<size_t N>
	inline RDFClassHistogram<N>& RDFClassHistogram<N>::operator=( const RDFClassHistogram<N>& other )
	{
		if( this != &other ) {
			_numSamples = other._numSamples;
			::memcpy( _bin, other._bin, sizeof( size_t ) * N );
		}
		return *this;
	}

	template<size_t N>
	inline RDFClassHistogram<N>& RDFClassHistogram<N>::operator+=( const RDFClassHistogram<N>& other )
	{
		_numSamples += other._numSamples;
		for( size_t i = 0; i < N; i++ )
			_bin [ i ] += other._bin[ i ];
		return *this;
	}

	template<size_t N>
	inline size_t RDFClassHistogram<N>::sampleCount() const
	{
		return _numSamples;
	}

	template<size_t N>
	inline float RDFClassHistogram<N>::entropy() const
	{
		float ret = 0.0;;
		if( !_numSamples ) return 0.0f;

		for( size_t i = 0; i < N; i++ )
		{
			if( _bin[ i ] ) {
				float p = probability( i );
				ret -= p * Math::log2( p );
			}
		}
		return ret;
	}

	template<size_t N>
	inline void RDFClassHistogram<N>::addSample( size_t classLabel )
	{
		_bin[ classLabel ]++;
		_numSamples++;
	}

	template<size_t N>
	inline void RDFClassHistogram<N>::clear()
	{
		_numSamples = 0;
		::memset( _bin, 0, sizeof( size_t  ) * N );
	}

    template<size_t N>
    std::ostream& operator<<( std::ostream& out, const RDFClassHistogram<N>& hist )
    {
		for( size_t i = 0; i < N; i++ )
			out << " | " <<  i << " : " << hist.probability( i );
		out << " |";
        return out;
    }

}

#endif
