#ifndef SIMDSSE2_H
#define SIMDSSE2_H

#include <cvt/util/SIMDSSE.h>

namespace cvt {

	class SIMDSSE2 : public SIMDSSE {
		friend class SIMD;

		protected:
			SIMDSSE2() {};

        public:
			using SIMDSSE::Mul;
			virtual void MulValue1fx( Fixed* dst, const Fixed* src, Fixed value, size_t n ) const;			
			virtual void MulAddValue1fx( Fixed* dst, const Fixed* src, Fixed value, size_t n ) const;
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
 
			virtual void pyrdownHalfHorizontal_1u8_to_1u16( uint16_t* dst, const uint8_t* src, size_t n ) const;
			virtual void pyrdownHalfVertical_1u16_to_1u8( uint8_t* dst, uint16_t* rows[ 5 ], size_t n ) const;

			virtual float harrisResponse1u8( const uint8_t* _src, size_t srcStride, size_t w, size_t h, const float k ) const;
			virtual float harrisResponse1u8( float & xx, float & xy, float & yy, const uint8_t* _src, size_t srcStride, size_t w, size_t h, const float k ) const;
			virtual float harrisResponseCircular1u8( float & xx, float & xy, float & yy, const uint8_t* _src, size_t srcStride, const float k ) const;

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
