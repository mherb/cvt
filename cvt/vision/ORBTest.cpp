#include <cvt/vision/ORB.h>
#include <cvt/util/CVTTest.h>
#include <cvt/io/Resources.h>

namespace cvt {

    bool _centroidAngleTest()
    {
        cvt::Resources r;
        cvt::Image img( r.find( "lena_g.png" ) );
        cvt::ORB orb( img, 1, 0.5f, 50 );

        cvt::Image testImage( 640, 480, cvt::IFormat::GRAY_UINT8 );

        testImage.fill( cvt::Color( 1.0f, 1.0f, 1.0f, 1.0f ) );

        cvt::Image ii;
        testImage.integralImage( ii );

        size_t stride;
        float * ptr = ii.map<float>( &stride );

        cvt::ORBFeature feature( 320, 240 );

        orb.centroidAngle( feature, ptr, stride );

        std::cout << "Feature Angle: " << Math::rad2Deg( feature.angle ) << std::endl;

        return false;
    }
}

BEGIN_CVTTEST( orb )

	bool tmp, result = true;

	tmp = cvt::_centroidAngleTest();
	CVTTEST_PRINT( "centroidAngle(...)", tmp );
	result &= tmp;

	return result;

END_CVTTEST