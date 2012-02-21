/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
/*
 * File:   SIMDSSE3.h
 * Author: sebi
 *
 * Created on August 1, 2011, 8:50 AM
 */

#ifndef CVT_SIMDSSE3_H
#define	CVT_SIMDSSE3_H

#include <cvt/util/SIMDSSE2.h>

namespace cvt {

    class SIMDSSE3 : public SIMDSSE2 {

        friend class SIMD;

      protected:
        SIMDSSE3() {}

      public:
        virtual std::string name() const;
        virtual SIMDType type() const;

    };

    inline std::string SIMDSSE3::name() const
	{
		return "SIMD-SSE3";
	}

	inline SIMDType SIMDSSE3::type() const
	{
		return SIMD_SSE3;
	}

}

#endif

