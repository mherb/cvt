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

	struct FeatureDescriptor : public Feature
	{
		FeatureDescriptor( const Feature& f ) : Feature( f )
		{
		}

		virtual ~FeatureDescriptor()
		{
		}

		virtual size_t length() const = 0;
		virtual const uint8_t* ptr() const = 0;
		virtual FeatureDescriptorComparator compareType() const = 0;
	};

	template<size_t N, typename T, FeatureDescriptorComparator CMPTYPE>
	class FeatureDescriptorDistance;

	template<size_t N, typename T, FeatureDescriptorComparator CMPTYPE>
	struct FeatureDescriptorInternal : public FeatureDescriptor
	{
		FeatureDescriptorInternal( const Feature& f ) : FeatureDescriptor( f )
		{
		}

		FeatureDescriptorInternal( float x, float y, float a, size_t o, float sc ) : FeatureDescriptor( x, y, a, o, sc )
		{
		}

		virtual ~FeatureDescriptorInternal()
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

		virtual FeatureDescriptorComparator compareType() const
		{
			return CMPTYPE;
		}

		float distance( const FeatureDescriptorInternal<N,T,CMPTYPE>& desc ) const
		{
			return FeatureDescriptorDistance<N,T,CMPTYPE>::distance( *this, desc );
		}

		T desc[ N ];
	};


	template<size_t N, typename T, FeatureDescriptorComparator CMPTYPE>
	struct FeatureDescriptorDistance
	{
		static float distance( const FeatureDescriptorInternal<N,T,CMPTYPE>& a, const FeatureDescriptorInternal<N,T,CMPTYPE>& b )
		{
			throw CVTException( "Error" );
			return 0;
		}
	};

	/* float, SSD */
	template<size_t N>
	struct FeatureDescriptorDistance<N, float, FEATUREDESC_CMP_SSD>
	{
		static float distance( const FeatureDescriptorInternal<N,float, FEATUREDESC_CMP_SSD>& a, const FeatureDescriptorInternal<N,float, FEATUREDESC_CMP_SSD>& b )
		{
			return SIMD::instance()->SSD( a.desc, b.desc, N );
		}
	};

	/* uint8_t, SSD */
	template<size_t N>
	struct FeatureDescriptorDistance<N, uint8_t, FEATUREDESC_CMP_SSD>
	{
		static float distance( const FeatureDescriptorInternal<N,uint8_t, FEATUREDESC_CMP_SSD>& a, const FeatureDescriptorInternal<N,uint8_t, FEATUREDESC_CMP_SSD>& b )
		{
			return SIMD::instance()->SSD( a.desc, b.desc, N );
		}
	};

	/* float, SAD */
	template<size_t N>
	struct FeatureDescriptorDistance<N, float, FEATUREDESC_CMP_SAD>
	{
		static float distance( const FeatureDescriptorInternal<N,float, FEATUREDESC_CMP_SAD>& a, const FeatureDescriptorInternal<N,float, FEATUREDESC_CMP_SAD>& b )
		{
			return SIMD::instance()->SAD( a.desc, b.desc, N );
		}
	};

	/* uint8_t, SAD */
	template<size_t N>
	struct FeatureDescriptorDistance<N, uint8_t, FEATUREDESC_CMP_SAD>
	{
		static float distance( const FeatureDescriptorInternal<N,uint8_t, FEATUREDESC_CMP_SAD>& a, const FeatureDescriptorInternal<N,uint8_t, FEATUREDESC_CMP_SAD>& b )
		{
			return SIMD::instance()->SAD( a.desc, b.desc, N );
		}
	};

	/* uint8_t, hamming */
	template<size_t N>
	struct FeatureDescriptorDistance<N, uint8_t, FEATUREDESC_CMP_HAMMING>
	{
		static float distance( const FeatureDescriptorInternal<N,uint8_t, FEATUREDESC_CMP_HAMMING>& a, const FeatureDescriptorInternal<N,uint8_t, FEATUREDESC_CMP_HAMMING>& b )
		{
			return SIMD::instance()->hammingDistance( a.desc, b.desc, N );
		}
	};

}

#endif
