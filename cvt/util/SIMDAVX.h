/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef SIMDAVX_H
#define SIMDAVX_H

#include <cvt/util/SIMDSSE42.h>

namespace cvt {

	class SIMDAVX : public SIMDSSE42 {
		friend class SIMD;

		protected:
			SIMDAVX() {}

		public:
			virtual std::string name() const;
			virtual SIMDType type() const;
	};

	inline std::string SIMDAVX::name() const
	{
		return "SIMD-AVX";
	}

	inline SIMDType SIMDAVX::type() const
	{
		return SIMD_AVX;
	}
}

#endif
