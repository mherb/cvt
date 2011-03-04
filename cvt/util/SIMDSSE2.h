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
			virtual void Mul( Fixed* dst, const Fixed* src, Fixed value, size_t n ) const;
			using SIMDSSE::MulAdd;
			virtual void MulAdd( Fixed* dst, const Fixed* src, Fixed value, size_t n ) const;
			using SIMDSSE::SAD;
			virtual size_t SAD( uint8_t const* src1, uint8_t const* src2, const size_t n ) const;

			virtual void ConvolveClampSet1fx( Fixed* dst, uint8_t const* src, const size_t width, const Fixed* weights, const size_t wn ) const;
			virtual void ConvolveClampAdd1fx( Fixed* dst, uint8_t const* src, const size_t width, const Fixed* weights, const size_t wn ) const;
			virtual void ConvolveClampSet4fx( Fixed* dst, uint8_t const* src, const size_t width, const Fixed* weights, const size_t wn ) const;
			virtual void ConvolveClampAdd4fx( Fixed* dst, uint8_t const* src, const size_t width, const Fixed* weights, const size_t wn ) const;

			virtual void ConvolveClampVert_fx_to_u8( uint8_t* dst, const Fixed** bufs, const Fixed* weights, size_t numw, size_t width ) const;
			virtual void ConvolveClampVert_f( float* dst, const float** bufs, const float* weights, size_t numw, size_t width ) const;

			void Conv_fx_to_u8( uint8_t* dst, const Fixed* src, const size_t n ) const;

			virtual void Conv_YUYVu8_to_RGBAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const;
			virtual void Conv_YUYVu8_to_BGRAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const;
			virtual void Conv_UYVYu8_to_RGBAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const;
			virtual void Conv_UYVYu8_to_BGRAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const;
        
            void prefixSum1_u8_to_f( float * dst, size_t dstStride, const uint8_t * src, size_t srcStride, size_t width, size_t height ) const;
            void prefixSumSqr1_u8_to_f( float * dst, size_t dStride, const uint8_t * src, size_t srcStride, size_t width, size_t height ) const;
        
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
