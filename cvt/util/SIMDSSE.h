#ifndef SIMDSSE_H
#define SIMDSSE_H

#include "util/SIMD.h"

namespace cvt {

	class SIMDSSE : public SIMD {
		friend class SIMD;

		private:
			SIMDSSE() {};

		public:
			using SIMD::Add;
			using SIMD::Sub;
			using SIMD::Mul;
			using SIMD::Div;

			/* memory blocks */
			virtual void Add( float* dst, float const* src1, float const* src2, const size_t n ) const;
			virtual void Sub( float* dst, float const* src1, float const* src2, const size_t n ) const;
			virtual void Mul( float* dst, float const* src1, float const* src2, const size_t n ) const;
			virtual void Div( float* dst, float const* src1, float const* src2, const size_t n ) const;

			virtual void Add( float* dst, float const* src1, const float v, const size_t n ) const;
			virtual void Sub( float* dst, float const* src1, const float v, const size_t n ) const;
			virtual void Mul( float* dst, float const* src1, const float v, const size_t n ) const;
			virtual void Div( float* dst, float const* src1, const float v, const size_t n ) const;

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
