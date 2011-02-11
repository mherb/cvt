#ifndef SIMDSSE2_H
#define SIMDSSE2_H

#include <cvt/util/SIMDSSE.h>

namespace cvt {

	class SIMDSSE2 : public SIMDSSE {
		friend class SIMD;
		friend class SIMDSSE41;

		private:
			SIMDSSE2() {};

			using SIMDSSE::Mul;
			using SIMDSSE::MulAdd;
			void Mul( Fixed* dst, const Fixed* src, Fixed value, size_t n ) const;
			void MulAdd( Fixed* dst, const Fixed* src, Fixed value, size_t n ) const;

			virtual void ConvolveClampSet4fx( Fixed* dst, uint8_t const* src, const size_t width, const Fixed* weights, const size_t wn ) const;
			virtual void ConvolveClampAdd4fx( Fixed* dst, uint8_t const* src, const size_t width, const Fixed* weights, const size_t wn ) const;

			virtual void Conv_YUYVu8_to_RGBAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const;
			virtual void Conv_YUYVu8_to_BGRAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const;
			virtual void Conv_UYVYu8_to_RGBAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const;
			virtual void Conv_UYVYu8_to_BGRAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const;
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
