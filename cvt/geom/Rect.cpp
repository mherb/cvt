/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/geom/Rect.h>
#include <cvt/util/CVTTest.h>

BEGIN_CVTTEST(rect)
    cvt::Rectf r( 0.0f, 0.0f, 100.0f, 100.0f );
    std::cout << r << std::endl;

    std::cout << r.contains( r ) << std::endl;
    std::cout << r.intersects( r ) << std::endl;
    return true;
END_CVTTEST

