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
#include <cvt/util/SIMD.h>

namespace cvt {

	enum FeatureDescriptorComparator {
		FEATUREDESC_CMP_SSD		= 0,
		FEATUREDESC_CMP_SAD		= 1,
		FEATUREDESC_CMP_HAMMING = 2,
	};

	template<size_t N, typename T>
	struct FeatureDescriptorBase : public Feature
	{
		FeatureDescriptorBase( float x, float y, float a, size_t o, float sc ) : Feature( x, y, a, o, sc )
		{
		}

		float distance( const FeatureDescriptorBase<N,T>& desc ) const;

		T desc[ N ];
	};

	template<size_t N, typename T, FeatureDescriptorComparator CMPTYPE>
	struct FeatureDescriptor : public FeatureDescriptorBase<N,T>
	{
		float distance( const FeatureDescriptorBase<N,T>& other ) const
		{
		}
	};

	/* float, SSD */
	template<size_t N>
	struct FeatureDescriptor<N, float, FEATUREDESC_CMP_SSD> : public FeatureDescriptorBase<N,float>
	{
		float distance( const FeatureDescriptorBase<N,float>& other ) const
		{
			return SIMD::instance()->SSD( this->desc, other.desc, N );
		}
	};

	/* uint8_t, SSD */
	template<size_t N>
	struct FeatureDescriptor<N, uint8_t, FEATUREDESC_CMP_SSD> : public FeatureDescriptorBase<N,uint8_t>
	{
		float distance( const FeatureDescriptorBase<N,uint8_t>& other ) const
		{
			return SIMD::instance()->SSD( this->desc, other.desc, N );
		}
	};

	/* float, SAD */
	template<size_t N>
	struct FeatureDescriptor<N, float, FEATUREDESC_CMP_SAD> : public FeatureDescriptorBase<N,float>
	{
		float distance( const FeatureDescriptorBase<N,float>& other ) const
		{
			return SIMD::instance()->SAD( this->desc, other.desc, N );
		}
	};

	/* uint8_t, SAD */
	template<size_t N>
	struct FeatureDescriptor<N, uint8_t, FEATUREDESC_CMP_SAD> : public FeatureDescriptorBase<N,uint8_t>
	{
		float distance( const FeatureDescriptorBase<N,uint8_t>& other ) const
		{
			return SIMD::instance()->SAD( this->desc, other.desc, N );
		}
	};

	/* uint8_t, hamming */
	template<size_t N>
	struct FeatureDescriptor<N, uint8_t, FEATUREDESC_CMP_HAMMING> : public FeatureDescriptorBase<N,uint8_t>
	{
		float distance( const FeatureDescriptorBase<N,uint8_t>& other ) const
		{
			return SIMD::instance()->hammingDistance( this->desc, other.desc, N );
		}
	};
}

#endif
