/*
 *  Range.cpp
 *  CVTools
 *
 *  Created by Sebastian Klose on 16.08.10.
 *  Copyright 2010. All rights reserved.
 *
 */

#include "Range.h"
#include "CVTTest.h"

BEGIN_CVTTEST(range)
cvt::Rangef r( -100.0f, 100.0f);
std::cout << r << std::endl;
return true;
END_CVTTEST

