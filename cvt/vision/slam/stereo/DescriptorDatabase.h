/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_DESCRIPTOR_DATA_BASE_H
#define CVT_DESCRIPTOR_DATA_BASE_H

#include <vector>
#include <cvt/vision/features/FeatureDescriptor.h>

namespace cvt
{
	class DescriptorDatabase
	{
		public:
			DescriptorDatabase();
			~DescriptorDatabase();

			void						clear();
			void						addDescriptor( const FeatureDescriptor& desc, size_t id );
			const FeatureDescriptor&	operator[]( size_t id ) const;

			void descriptorsForIds( std::vector<FeatureDescriptor*>& descriptors,
									const std::vector<size_t>& ids );

		private:
			std::vector<FeatureDescriptor*>	_descriptors;
	};

	inline DescriptorDatabase::DescriptorDatabase()
	{
		_descriptors.reserve( 2000 );
	}

	inline DescriptorDatabase::~DescriptorDatabase()
	{
		for( size_t i = 0; i < _descriptors.size(); i++ ){
			if( _descriptors[ i ] != 0 )
				delete _descriptors[ i ];
		}
	}

	inline void DescriptorDatabase::clear()
	{
		_descriptors.clear();
	}

	inline void DescriptorDatabase::addDescriptor( const FeatureDescriptor& d, size_t id )
	{
		size_t numDesc = _descriptors.size();
		if( id < numDesc ){
			// already have this id -> update this descriptor
			if( _descriptors[ id ] != 0 ){
				*_descriptors[ id ] = d;
			} else {
				_descriptors[ id ] = d.clone();
			}
			return;
		}

		if( id == numDesc ){
			_descriptors.push_back( d.clone() );
			return;
		}

		// handle non-consecutive id insertion
		size_t lastId = numDesc;
		_descriptors.resize( ( id+1 ) );
		while( lastId < id )
			_descriptors[ lastId++ ] = 0;
		_descriptors[ id ] = d.clone();
	}

	inline const FeatureDescriptor& DescriptorDatabase::operator[]( size_t id ) const
	{
		const FeatureDescriptor* des = _descriptors[ id ];
		if( des == 0 ){
			throw CVTException( "descriptor for requested id does not exist" );
		}
		return *des;
	}

	inline void DescriptorDatabase::descriptorsForIds( std::vector<FeatureDescriptor*>& descriptors,
													   const std::vector<size_t>& ids )
	{
		descriptors.resize( ids.size() );
		for( size_t i = 0; i < ids.size(); ++i ){
			descriptors[ i ] = _descriptors[ ids[ i ] ];
		}
	}
}

#endif
