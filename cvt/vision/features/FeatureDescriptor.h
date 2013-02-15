/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_FEATUREDESCRPTOR_H
#define CVT_FEATUREDESCRPTOR_H

#include <cvt/vision/features/Feature.h>
#include <cvt/util/Exception.h>
#include <cvt/util/SIMD.h>

namespace cvt {
	enum FeatureDescriptorComparator {
		FEATUREDESC_CMP_SSD		= 0,
		FEATUREDESC_CMP_SAD		= 1,
		FEATUREDESC_CMP_HAMMING = 2,
	};

	class FeatureDescriptor : public Feature
	{
		public:
			//FeatureDescriptor( const Feature& f ) : Feature( f )
			//{
			//}

			virtual ~FeatureDescriptor()
			{
			}

			virtual size_t length() const = 0;
			virtual const uint8_t* ptr() const = 0;
			virtual FeatureDescriptorComparator compareType() const = 0;
	};


	template<size_t N, typename T>
	struct FeatureDescriptorInternalBase : public FeatureDescriptor
	{
		FeatureDescriptorInternalBase( const Feature& f ) : Feature( f )
		{
		}

		FeatureDescriptorInternalBase( float x, float y, float a, size_t o, float sc ) : Feature( x, y, a, o, sc )
		{
		}

		virtual ~FeatureDescriptorInternalBase()
		{
		}

		virtual size_t length() const
		{
			return sizeof( T ) * N;
		}

		virtual const uint8_t* ptr() const
		{
			return ( uint8_t* ) desc;
		}

		float distance( const FeatureDescriptorInternalBase<N,T>& desc ) const;

		T desc[ N ];
	};

	template<size_t N, typename T, FeatureDescriptorComparator CMPTYPE>
	struct FeatureDescriptorInternal : public FeatureDescriptorInternalBase<N,T>
	{

		virtual FeatureDescriptorComparator compareType()
		{
			return CMPTYPE;
		}

		float distance( const FeatureDescriptorInternalBase<N,T>& other ) const
		{
			throw CVTException( "Error" );
			return 0;
		}
	};

	/* float, SSD */
	template<size_t N>
	struct FeatureDescriptorInternal<N, float, FEATUREDESC_CMP_SSD> : public FeatureDescriptorInternalBase<N,float>
	{
		float distance( const FeatureDescriptorInternalBase<N,float>& other ) const
		{
			return SIMD::instance()->SSD( this->desc, other.desc, N );
		}
	};

	/* uint8_t, SSD */
	template<size_t N>
	struct FeatureDescriptorInternal<N, uint8_t, FEATUREDESC_CMP_SSD> : public FeatureDescriptorInternalBase<N,uint8_t>
	{
		float distance( const FeatureDescriptorInternalBase<N,uint8_t>& other ) const
		{
			return SIMD::instance()->SSD( this->desc, other.desc, N );
		}
	};

	/* float, SAD */
	template<size_t N>
	struct FeatureDescriptorInternal<N, float, FEATUREDESC_CMP_SAD> : public FeatureDescriptorInternalBase<N,float>
	{
		float distance( const FeatureDescriptorInternalBase<N,float>& other ) const
		{
			return SIMD::instance()->SAD( this->desc, other.desc, N );
		}
	};

	/* uint8_t, SAD */
	template<size_t N>
	struct FeatureDescriptorInternal<N, uint8_t, FEATUREDESC_CMP_SAD> : public FeatureDescriptorInternalBase<N,uint8_t>
	{
		float distance( const FeatureDescriptorInternalBase<N,uint8_t>& other ) const
		{
			return SIMD::instance()->SAD( this->desc, other.desc, N );
		}
	};

	/* uint8_t, hamming */
	template<size_t N>
	struct FeatureDescriptorInternal<N, uint8_t, FEATUREDESC_CMP_HAMMING> : public FeatureDescriptorInternalBase<N,uint8_t>
	{
		float distance( const FeatureDescriptorInternalBase<N,uint8_t>& other ) const
		{
			return SIMD::instance()->hammingDistance( this->desc, other.desc, N );
		}
	};
}

#endif
