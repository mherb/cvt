#include <cvt/vision/ORB.h>
#include <cvt/util/CVTTest.h>
#include <cvt/io/Resources.h>
#include <cvt/math/Matrix.h>

#include <cvt/gfx/ifilter/ITransform.h>

#include <cvt/gfx/ifilter/Homography.h>

namespace cvt {

    bool _centroidAngleTest()
    {
        cvt::Resources r;

        bool result = true;

        for( size_t i = 0; i < 360; i+=5 ){
            String fileIn( "bw" );
            fileIn += i;
            fileIn += ".png";
            //std::cout << "File : " << fileIn << std::endl;
            cvt::Image _img( r.find( fileIn ) );

            cvt::Image img;
            _img.convert( img, IFormat::GRAY_UINT8 );
            cvt::ORB orb( img, 1, 0.5f, 50 );

            cvt::Image ii;
            img.integralImage( ii );

            size_t stride;
            float * ptr = ii.map<float>( &stride );

            cvt::ORBFeature feature( img.width() / 2.0f, img.height() / 2.0f );

            orb.centroidAngle( feature, ptr, stride );

            //std::cout << "Feature Angle: " << Math::rad2Deg( feature.angle ) << std::endl;

            ii.unmap<float>( ptr );

        }

        return true;
    }
}

BEGIN_CVTTEST( orb )

	bool tmp, result = true;

	tmp = cvt::_centroidAngleTest();
	CVTTEST_PRINT( "centroidAngle(...)", tmp );
	result &= tmp;

	return result;

END_CVTTEST