#ifndef SIMDSSE41_H
#define SIMDSSE41_H

#include <cvt/util/SIMDSSE2.h>

namespace cvt {

	class SIMDSSE41 : public SIMDSSE2 {
		friend class SIMD;

		private:
			SIMDSSE41() {};

//			using SIMDSSE2::MulAdd;
//			virtual void MulAdd( Fixed* _dst, const Fixed* _src, Fixed value, size_t n ) const;

			virtual void Conv_XXXAu8_to_XXXAf( float* dst, uint8_t const* src, const size_t n ) const;
			virtual void Conv_XYZAu8_to_ZYXAf( float* dst, uint8_t const* src, const size_t n ) const;

		public:
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
