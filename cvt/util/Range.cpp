/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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

