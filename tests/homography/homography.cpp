#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/Homography.h>
#include <cvt/gfx/ifilter/ITransform.h>
#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>
#include <cvt/util/Time.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <string>

using namespace cvt;

int main()
{
	std::string dataFolder( DATA_FOLDER );
	std::string inputFile( dataFolder + "/lena.png" );

	try {
		Image img;
		img.load( inputFile.c_str() );

		Image imgf( img.width(), img.height(), IFormat::RGBA_FLOAT );
		Image out( img.width(), img.height(), IFormat::RGBA_FLOAT );
		Image out2( img.width(), img.height(), IFormat::RGBA_FLOAT );
		img.convert( imgf );

		Homography hfilter;

        cvt::Matrix3f H;
		H.setHomography( Math::deg2Rad( 23.0f ), 0.0f, 1.5f, 0.8f, 10.0f, 10.0f, 0.0f, 0.0f );
		if( !H.inverseSelf() ){
            std::cerr << "Could not invert" << std::endl;
            return 0;
        }

		Color black( 0.0f, 0.0f, 0.0f, 1.0f );
		Time t;
		t.reset();
		hfilter.apply( out, imgf, H, black );
		std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;
		out.save( "outhomography.png" );

		// otherwise denormalized float stuff kicks in
		out2.fill( Color::BLACK );
		t.reset();
		ITransform::apply( out2, imgf, H );
		std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;
		out2.save( "outhomography2.png" );

	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
