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
			using SIMD::MulAdd;
			using SIMD::MulSub;

			/* memory blocks */
			virtual void Add( float* dst, float const* src1, float const* src2, const size_t n ) const;
			virtual void Sub( float* dst, float const* src1, float const* src2, const size_t n ) const;
			virtual void Mul( float* dst, float const* src1, float const* src2, const size_t n ) const;
			virtual void Div( float* dst, float const* src1, float const* src2, const size_t n ) const;

			virtual void Add( float* dst, float const* src1, const float v, const size_t n ) const;
			virtual void Sub( float* dst, float const* src1, const float v, const size_t n ) const;
			virtual void Mul( float* dst, float const* src1, const float v, const size_t n ) const;
			virtual void Div( float* dst, float const* src1, const float v, const size_t n ) const;

			virtual void MulAdd( float* dst, float const* src1, const float value, const size_t n ) const;
			virtual void MulSub( float* dst, float const* src1, const float value, const size_t n ) const;


			/*shuffle*/
			virtual void Conv_XYZAf_to_ZYXAf( float* dst, float const* src, const size_t n ) const;

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
