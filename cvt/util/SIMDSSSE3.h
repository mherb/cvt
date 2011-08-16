/*
 * File:   SIMDSSSE3.h
 * Author: sebi
 *
 * Created on August 1, 2011, 8:48 AM
 */

#ifndef CVT_SIMDSSSE3_H
#define	CVT_SIMDSSSE3_H

#include <cvt/util/SIMDSSE3.h>

namespace cvt {

    class SIMDSSSE3 : public SIMDSSE3 {

		friend class SIMD;

      protected:
        SIMDSSSE3() {}

      public:
		virtual void Conv_XYZAu8_to_ZYXAu8( uint8_t* dst, uint8_t const* src, const size_t n ) const;
		virtual size_t hammingDistance(const uint8_t* src1, const uint8_t* src2, size_t n) const;

        virtual std::string name() const;

        virtual SIMDType type() const;

    };

    inline std::string SIMDSSSE3::name() const
	{
		return "SIMD-SSSE3";
	}

	inline SIMDType SIMDSSSE3::type() const
	{
		return SIMD_SSSE3;
	}

}

#endif	/* SIMDSSSE3_H */

