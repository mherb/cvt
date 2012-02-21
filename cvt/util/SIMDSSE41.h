/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef SIMDSSE41_H
#define SIMDSSE41_H

#include <cvt/util/SIMDSSSE3.h>

namespace cvt {

	class SIMDSSE41 : public SIMDSSSE3 {
		friend class SIMD;

		protected:
			SIMDSSE41() {};

   		public:
			virtual void Conv_XXXAu8_to_XXXAf( float* dst, uint8_t const* src, const size_t n ) const;
			virtual void Conv_XYZAu8_to_ZYXAf( float* dst, uint8_t const* src, const size_t n ) const;

            virtual std::string name() const;
			virtual SIMDType type() const;
	};

	inline std::string SIMDSSE41::name() const
	{
		return "SIMD-SSE41";
	}

	inline SIMDType SIMDSSE41::type() const
	{
		return SIMD_SSE41;
	}
}

#endif
