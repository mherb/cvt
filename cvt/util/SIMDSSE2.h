#ifndef SIMDSSE2_H
#define SIMDSSE2_H

#include <cvt/util/SIMDSSE.h>

namespace cvt {

	class SIMDSSE2 : public SIMDSSE {
		friend class SIMD;
		friend class SIMDSSE41;

		private:
			SIMDSSE2() {};

			using SIMDSSE::Conv_XXXAf_to_XXXAu8;
			using SIMDSSE::Conv_XYZAf_to_ZYXAu8;

		public:
			virtual std::string name() const;
			virtual SIMDType type() const;
	};

	inline std::string SIMDSSE2::name() const
	{
		return "SIMD-SSE2";
	}

	inline SIMDType SIMDSSE2::type() const
	{
		return SIMD_SSE2;
	}
}

#endif
