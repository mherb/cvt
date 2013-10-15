#include <cvt/geom/Rect.h>
#include <cvt/util/CVTTest.h>

BEGIN_CVTTEST(rect)
    cvt::Rectf r( 0.0f, 0.0f, 100.0f, 100.0f );
    std::cout << r << std::endl;

    std::cout << r.contains( r ) << std::endl;
    std::cout << r.intersects( r ) << std::endl;
    return true;
END_CVTTEST

