#ifndef SIMDSSE42_H
#define SIMDSSE42_H

#include <cvt/util/SIMDSSE41.h>

namespace cvt {

	class SIMDSSE42 : public SIMDSSE41 {
		friend class SIMD;

		protected:
			SIMDSSE42()	{}

		public:
			virtual size_t hammingDistance(const uint8_t* src1, const uint8_t* src2, size_t n) const;

			virtual std::string name() const;
			virtual SIMDType type() const;
	};

    inline std::string SIMDSSE42::name() const {
        return "SIMD-SSE42";
    }

    inline SIMDType SIMDSSE42::type() const {
        return SIMD_SSE42;
    }
}

#endif
