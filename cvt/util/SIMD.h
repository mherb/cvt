#ifndef SIMD_H
#define SIMD_H
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <string>

#include <cvt/gfx/IScaleFilter.h>

namespace cvt {
	enum SIMDType {
		SIMD_BASE = 0,
		SIMD_SSE,
		SIMD_SSE2,
		SIMD_SSE41,
		SIMD_SSE42,
		SIMD_AVX,
		SIMD_BEST
	};

    class SIMD
	{
		protected:
			SIMD() {};
			SIMD( const SIMD& ) {};
        
		public:
			virtual ~SIMD() {};

			virtual void Memcpy( uint8_t* dst, uint8_t const* src, const size_t n ) const;

			/* set to constant */
			virtual void SetValueU8( uint8_t* dst, const uint8_t value, const size_t n ) const;
			virtual void SetValueU16( uint16_t* dst, const uint16_t value, const size_t n ) const;
			virtual void SetValueU32( uint32_t* dst, const uint32_t value, const size_t n ) const;
			virtual void SetValue1f( float* dst, const float value, const size_t n ) const;
			virtual void SetValue2f( float* dst, const float (&value)[ 2 ], const size_t n ) const;
			virtual void SetValue4f( float* dst, const float (&value)[ 4 ], const size_t n ) const;

			/* memory block and constants */
			virtual void Add( float* dst, float const* src, const float value, const size_t n ) const;
			virtual void Sub( float* dst, float const* src, const float value, const size_t n ) const;
			virtual void Mul( float* dst, float const* src, const float value, const size_t n ) const;
			virtual void Div( float* dst, float const* src, const float value, const size_t n ) const;
        
			virtual void Add( float* dst, float const* src, const float (&value)[ 4 ], const size_t n ) const;
			virtual void Sub( float* dst, float const* src, const float (&value)[ 4 ], const size_t n ) const;
			virtual void Mul( float* dst, float const* src, const float (&value)[ 4 ], const size_t n ) const;
			virtual void Div( float* dst, float const* src, const float (&value)[ 4 ], const size_t n ) const;

			virtual void MulAdd( float* dst, float const* src1, const float value, const size_t n ) const;
			virtual void MulSub( float* dst, float const* src1, const float value, const size_t n ) const;

			virtual void MulAdd( float* dst, float const* src1, const float (&value)[ 4 ], const size_t n ) const;
			virtual void MulSub( float* dst, float const* src1, const float (&value)[ 4 ], const size_t n ) const;

			/* memory blocks */
                        /* floating point numbers */
			virtual void Add( float* dst, float const* src1, float const* src2, const size_t n ) const;
			virtual void Sub( float* dst, float const* src1, float const* src2, const size_t n ) const;
			virtual void Mul( float* dst, float const* src1, float const* src2, const size_t n ) const;
			virtual void Div( float* dst, float const* src1, float const* src2, const size_t n ) const;

			/* Fixed point numbers */
			virtual void Mul( Fixed * dst, const Fixed * src, Fixed value, size_t n ) const;
			virtual void MulAdd( Fixed* dst, const Fixed* src, Fixed value, size_t n ) const;
			virtual void Mul( Fixed * dst, const uint8_t* src, Fixed value, size_t n ) const;
			virtual void MulAdd( Fixed* dst, const uint8_t* src, Fixed value, size_t n ) const;
                    
			/* memory block reductions */
			virtual float SSD( float const* src1, float const* src2, const size_t n ) const;
			virtual float SSD( uint8_t const* src1, uint8_t const* src2, const size_t n ) const;
			virtual float SAD( float const* src1, float const* src2, const size_t n ) const;
			virtual size_t SAD( uint8_t const* src1, uint8_t const* src2, const size_t n ) const;

			/* Infinite Impulse Response */
			/* IIR_#coefs_#pass_#direction_#channels_inputType */
			virtual void IIR4FwdHorizontal4Fx( Fixed* dst, const uint8_t * src, size_t width, const Fixed * n,
											   const Fixed * d, const Fixed & b1 ) const;

			virtual void IIR4BwdHorizontal4Fx( uint8_t * dst, const Fixed* fwdRes, const uint8_t * src, size_t w,
											   const Fixed * n, const Fixed * d, const Fixed & b ) const;

			virtual void IIR4FwdVertical4Fx( Fixed* buffer, const uint8_t * src, size_t sstride,
											 size_t h, const Fixed * n, const Fixed * d, const Fixed & b ) const;

			virtual void IIR4BwdVertical4Fx( uint8_t * dst, size_t dstride, Fixed* fwdRes,
											 size_t h, const Fixed * n, const Fixed * d, const Fixed & b ) const;

			virtual void ConvolveClampSet1f( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn ) const;
			virtual void ConvolveClampAdd1f( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn ) const;
			virtual void ConvolveClampSet2f( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn ) const;
			virtual void ConvolveClampAdd2f( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn ) const;
			virtual void ConvolveClampSet4f( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn ) const;
			virtual void ConvolveClampAdd4f( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn ) const;

			virtual void ConvolveClampSet1fx( Fixed* _dst, uint8_t const* _src, const size_t width, const Fixed* weights, const size_t wn ) const;
			virtual void ConvolveClampAdd1fx( Fixed* _dst, uint8_t const* _src, const size_t width, const Fixed* weights, const size_t wn ) const;
			virtual void ConvolveClampSet2fx( Fixed* _dst, uint8_t const* _src, const size_t width, const Fixed* weights, const size_t wn ) const;
			virtual void ConvolveClampAdd2fx( Fixed* _dst, uint8_t const* _src, const size_t width, const Fixed* weights, const size_t wn ) const;
			virtual void ConvolveClampSet4fx( Fixed* _dst, uint8_t const* _src, const size_t width, const Fixed* weights, const size_t wn ) const;
			virtual void ConvolveClampAdd4fx( Fixed* _dst, uint8_t const* _src, const size_t width, const Fixed* weights, const size_t wn ) const;

			virtual void ConvolveClampVert_fx_to_u8( uint8_t* dst, const Fixed** bufs, const Fixed* weights, size_t numw, size_t width ) const;
			virtual void ConvolveClampVert_f( float* dst, const float** bufs, const float* weights, size_t numw, size_t width ) const;

			virtual void ConvolveAdaptiveClamp1f( float* _dst, float const* _src, const size_t width, IConvolveAdaptivef* conva ) const;
			virtual void ConvolveAdaptiveClamp2f( float* _dst, float const* _src, const size_t width, IConvolveAdaptivef* conva ) const;
			virtual void ConvolveAdaptiveClamp4f( float* _dst, float const* _src, const size_t width, IConvolveAdaptivef* conva ) const;
			virtual void ConvolveAdaptive1Fixed( Fixed* _dst, uint8_t const* _src, size_t width, IConvolveAdaptiveFixed* conva ) const;
			virtual void ConvolveAdaptive2Fixed( Fixed* _dst, uint8_t const* _src, size_t width, IConvolveAdaptiveFixed* conva ) const;
			virtual void ConvolveAdaptive4Fixed( Fixed* _dst, uint8_t const* _src, size_t width, IConvolveAdaptiveFixed* conva ) const;


			virtual void Conv_f_to_u8( uint8_t* dst, float const* src, const size_t n ) const;
			virtual void Conv_fx_to_u8( uint8_t* dst, const Fixed* src, const size_t n ) const;
			virtual void Conv_fx_to_s16( int16_t* dst, const Fixed* src, const size_t n ) const;
			virtual void Conv_u8_to_f( float* dst, uint8_t const* src, const size_t n ) const;
			virtual void Conv_GRAYf_to_GRAYu8( uint8_t* _dst, const float* src, const size_t n ) const;
			virtual void Conv_GRAYf_to_XXXAf( float* dst, const float* src, const size_t n ) const;
			virtual void Conv_GRAYu8_to_XXXAu8( uint8_t* _dst, const uint8_t* src, const size_t n ) const;
			virtual void Conv_XXXAf_to_XXXAu8( uint8_t* dst, float const* src, const size_t n ) const;
			virtual void Conv_XYZAf_to_ZYXAu8( uint8_t* dst, float const* src, const size_t n ) const;
			virtual void Conv_XYZAf_to_ZYXAf( float* dst, float const* src, const size_t n ) const;
			virtual void Conv_XXXAu8_to_XXXAf( float* dst, uint8_t const* src, const size_t n ) const;
			virtual void Conv_XYZAu8_to_ZYXAf( float* dst, uint8_t const* src, const size_t n ) const;
			virtual void Conv_XYZAu8_to_ZYXAu8( uint8_t* dst, uint8_t const* src, const size_t n ) const;
			virtual void Conv_RGBAu8_to_GRAYf( float* _dst, uint8_t const* _src, const size_t n ) const;
			virtual void Conv_BGRAu8_to_GRAYf( float* _dst, uint8_t const* _src, const size_t n ) const;
			virtual void Conv_RGBAu8_to_GRAYu8( uint8_t* _dst, uint8_t const* _src, const size_t n ) const;
			virtual void Conv_BGRAu8_to_GRAYu8( uint8_t* _dst, uint8_t const* _src, const size_t n ) const;
			virtual void Conv_RGBAf_to_GRAYf( float* _dst, const float* _src, const size_t n ) const;
			virtual void Conv_BGRAf_to_GRAYf( float* _dst, const float* _src, const size_t n ) const;
                        virtual void Conv_XXXf_to_XXXAf(float * dst, const float* src, size_t n) const;
                        virtual void Conv_XXXu8_to_XXXAu8(uint8_t * dst, const uint8_t* src, size_t n) const;


			virtual void Conv_YUYVu8_to_RGBAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const;
			virtual void Conv_YUYVu8_to_BGRAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const;
			virtual void Conv_UYVYu8_to_RGBAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const;
			virtual void Conv_UYVYu8_to_BGRAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const;

			virtual void Conv_YUV420u8_to_RGBAu8( uint8_t* dst, const uint8_t* srcy, const uint8_t* srcu, const uint8_t* srcv, const size_t n ) const;
			virtual void Conv_YUV420u8_to_BGRAu8( uint8_t* dst, const uint8_t* srcy, const uint8_t* srcu, const uint8_t* srcv, const size_t n ) const;


			virtual void debayer_EVEN_RGGBu8_RGBAu8( uint32_t* dst, const uint32_t* src1, const uint32_t* src2, const uint32_t* src3, size_t n ) const;
			virtual void debayer_ODD_RGGBu8_RGBAu8( uint32_t* dst, const uint32_t* src1, const uint32_t* src2, const uint32_t* src3, size_t n ) const;

			virtual void debayer_EVEN_RGGBu8_BGRAu8( uint32_t* dst, const uint32_t* src1, const uint32_t* src2, const uint32_t* src3, size_t n ) const;
			virtual void debayer_ODD_RGGBu8_BGRAu8( uint32_t* dst, const uint32_t* src1, const uint32_t* src2, const uint32_t* src3, size_t n ) const;

			virtual void debayer_EVEN_RGGBu8_GRAYu8( uint32_t* dst, const uint32_t* src1, const uint32_t* src2, const uint32_t* src3, size_t n ) const;
			virtual void debayer_ODD_RGGBu8_GRAYu8( uint32_t* dst, const uint32_t* src1, const uint32_t* src2, const uint32_t* src3, size_t n ) const;
        
                        virtual size_t hammingDistance( const uint8_t* src1, const uint8_t* src2, size_t n ) const;
        
                        // prefix sum for 1 channel images
                        virtual void prefixSum1_u8_to_f( float * dst, size_t dstStride, const uint8_t * src, size_t srcStride, size_t width, size_t height ) const;

                        // prefix sum and square sum 
                        virtual void prefixSumSqr1_u8_to_f( float * dst, size_t dStride, const uint8_t * src, size_t srcStride, size_t width, size_t height ) const;


			virtual std::string name() const;
			virtual SIMDType type() const;

			static SIMD* get( SIMDType type = SIMD_BEST );
			static void  force( SIMDType type );
			static SIMD* instance();

		private:
			static SIMD* _simd;
	};

	inline void SIMD::Memcpy( uint8_t* dst, uint8_t const* src, const size_t n ) const
	{
		::memcpy( dst, src, n );
	}

	inline std::string SIMD::name() const
	{
		return "SIMD-BASE";
	}

	inline SIMDType SIMD::type() const
	{
		return SIMD_BASE;
	}
}

#endif
