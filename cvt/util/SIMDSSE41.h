#ifndef SIMDSSE41_H
#define SIMDSSE41_H

#include <cvt/util/SIMDSSE.h>

namespace cvt {

	class SIMDSSE41 : public SIMDSSE {
		friend class SIMD;

		private:
			SIMDSSE41() {};

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
