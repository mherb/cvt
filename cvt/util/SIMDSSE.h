#ifndef SIMDSSE_H
#define SIMDSSE_H

#include <cvt/util/SIMD.h>

namespace cvt {

	class SIMDSSE : public SIMD {
		friend class SIMD;
		friend class SIMDSSE2;

		protected:
			SIMDSSE() {};

		public:
			using SIMD::Add;
			using SIMD::Sub;
			using SIMD::Mul;
			using SIMD::Div;

			void Memcpy( uint8_t* dst, uint8_t const* src, const size_t n ) const;

			/* memory blocks */
			virtual void Add( float* dst, float const* src1, float const* src2, const size_t n ) const;
			virtual void Sub( float* dst, float const* src1, float const* src2, const size_t n ) const;
			virtual void Mul( float* dst, float const* src1, float const* src2, const size_t n ) const;
			virtual void Div( float* dst, float const* src1, float const* src2, const size_t n ) const;

			virtual void AddValue1f( float* dst, float const* src1, const float v, const size_t n ) const;
			virtual void SubValue1f( float* dst, float const* src1, const float v, const size_t n ) const;
			virtual void MulValue1f( float* dst, float const* src1, const float v, const size_t n ) const;
			virtual void DivValue1f( float* dst, float const* src1, const float v, const size_t n ) const;

			virtual void MulAddValue1f( float* dst, float const* src1, const float value, const size_t n ) const;
			virtual void MulSubValue1f( float* dst, float const* src1, const float value, const size_t n ) const;


			/*shuffle*/
			virtual void Conv_XYZAf_to_ZYXAf( float* dst, float const* src, const size_t n ) const;

			virtual void warpLinePerspectiveBilinear4f( float* dst, const float* src, size_t srcStride, size_t srcWidth, size_t srcHeight,
													    const float* point, const float* normal, const size_t n ) const;

			virtual std::string name() const;
			virtual SIMDType type() const;
	};

	inline std::string SIMDSSE::name() const
	{
		return "SIMD-SSE";
	}

	inline SIMDType SIMDSSE::type() const
	{
		return SIMD_SSE;
	}
}

#endif
