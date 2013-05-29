#ifndef CVT_DATAPOINTSET_H
#define CVT_DATAPOINTSET_H

#include <vector>
#include <string.h>

namespace cvt {
	template<typename T, size_t N = 1>
	class DataPointSet {
		public:
			DataPointSet();
			DataPointSet( const DataPointSet<T,N>& set );
			~DataPointSet();

			const T* operator[]( size_t idx ) const;
			      T* operator[]( size_t idx );
			size_t   size() const;

			void	 add( const T*, size_t n = N );
			void	 add( const T& value );
			void     remove( size_t index );

			//void     serialize( XMLNode* );
			//void     deserialize( XMLNode* );

		private:
			struct DataPoint {
				T data[ N ];
			};
			std::vector<DataPoint> _data;
	};

	template<typename T,size_t N>
	inline DataPointSet<T,N>::DataPointSet()
	{
	}

	template<typename T,size_t N>
	inline DataPointSet<T,N>::DataPointSet( const DataPointSet<T,N>& set )
	{
	}

	template<typename T,size_t N>
	inline DataPointSet<T,N>::~DataPointSet()
	{
	}

	template<typename T,size_t N>
	inline size_t DataPointSet<T,N>::size() const
	{
		return _data.size();
	}

	template<typename T,size_t N>
	inline const T* DataPointSet<T,N>::operator[]( size_t idx ) const
	{
		return _data[ idx ].data;
	}

	template<typename T,size_t N>
	inline T* DataPointSet<T,N>::operator[]( size_t idx )
	{
		return _data[ idx ].data;
	}

	template<typename T,size_t N>
	inline void DataPointSet<T,N>::add( const T* data, size_t n )
	{
		_data.resize( _data.size() + 1 );
		::memcpy( _data.back().data, data, sizeof( T ) * n );
	}

	template<typename T,size_t N>
	inline void DataPointSet<T,N>::add( const T& value )
	{
		_data.resize( _data.size() + 1 );
		_data.back().data[ 0 ] = value;
	}

	template<typename T, size_t N>
	inline void DataPointSet<T,N>::remove( size_t index )
	{
		_data.erase( _data.begin() + index );
	}

}

#endif
