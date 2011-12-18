#ifndef CVT_DESCRIPTOR_DATA_BASE_H
#define CVT_DESCRIPTOR_DATA_BASE_H

#include <vector>

namespace cvt
{
	template <class Desc>
	class DescriptorDatabase
	{
		public:
			DescriptorDatabase();
			~DescriptorDatabase();

			void addDescriptor( const Desc & desc, size_t id );

			const Desc & descriptor( size_t id ) const;

		private:
			std::vector<Desc*>	_descriptors;
	};

	template <class Desc>
	inline DescriptorDatabase<Desc>::DescriptorDatabase()
	{
	}

	template <class Desc>
	inline DescriptorDatabase<Desc>::~DescriptorDatabase()
	{
		for( size_t i = 0; i < _descriptors.size(); i++ ){
			if( _descriptors[ i ] != 0 )
				delete _descriptors[ i ];
		}
	}

	template <class Desc>
	inline void DescriptorDatabase<Desc>::addDescriptor( const Desc & d, size_t id )
	{
		size_t numDesc = _descriptors.size();
		if( id < numDesc ){
			if( _descriptors[ id ] != 0 ){
				*_descriptors[ id ] = d;
			} else {
				_descriptors[ id ] = new Desc( d );
			}
			return;
		}

		if( id == numDesc ){
			_descriptors.push_back( new Desc( d ) );
			return;
		}

		size_t lastId = numDesc;
		_descriptors.resize( ( id+1 ) );
		while( lastId < id )
			_descriptors[ lastId++ ] = 0;
		_descriptors[ id ] = new Desc( d );
	}

	template <class Desc>
	inline const Desc & DescriptorDatabase<Desc>::descriptor( size_t id ) const
	{
		return *_descriptors[ id ];
	}
}

#endif
