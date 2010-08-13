#include <iostream>

#include "math/Math.h"
#include "util/CVTTest.h"

namespace cvt {

BEGIN_CVTTEST( math )
    float f;
    float d;

    f = Math::mix( 0.0f, 1.0f, 0.5f );
    if( f != 0.5f )
	return false;
    std::cerr << "Mix with float OK" << std::endl;

    return true;
END_CVTTEST

}
