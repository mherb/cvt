#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/IFilterVector.h>
#include <cvt/gfx/ifilter/Homography.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <string>

using namespace cvt;

static IFilterVector8 calc_homography( float theta, float phi, float sx, float sy, float tx, float ty, float v1, float v2 )
{
	Eigen::Matrix3f h, ih;
	Eigen::Transform<float,2> affine;
	IFilterVector8 ret;

	affine = Eigen::Rotation2D<float>( Math::deg2Rad( phi ) );
	affine = Eigen::Scaling2f( sx, sy ) * affine;
	affine = Eigen::Rotation2D<float>( Math::deg2Rad( -phi ) ) * affine;
	affine = Eigen::Rotation2D<float>( Math::deg2Rad( theta ) ) * affine;

	h( 0, 0 ) = affine( 0, 0 );
	h( 0, 1 ) = affine( 0, 1 );
	h( 1, 0 ) = affine( 1, 0 );
	h( 1, 1 ) = affine( 1, 1 );
	h( 0, 2 ) = tx;
	h( 1, 2 ) = ty;
	h( 2, 0 ) = v1;
	h( 2, 1 ) = v2;
	h( 2, 2 ) = 1.0f;

	std::cout << h << std::endl;
	ih = h.inverse();
	std::cout << "DET " <<ih.determinant() << std::endl;
	ih /= powf( ih.determinant(), 1.0f / 3.0f );
	std::cout << "DET " <<ih.determinant() << std::endl;
	std::cout << ih << std::endl;

	ret[ 0 ] = ih( 0, 0 );
	ret[ 1 ] = ih( 0, 1 );
	ret[ 2 ] = ih( 0, 2 );
	ret[ 3 ] = ih( 1, 0 );
	ret[ 4 ] = ih( 1, 1 );
	ret[ 5 ] = ih( 1, 2 );
	ret[ 6 ] = ih( 2, 0 );
	ret[ 7 ] = ih( 2, 1 );
	return ret;
}


int main(int argc, char* argv[])
{
	std::string dataFolder(DATA_FOLDER);
	std::string inputFile(dataFolder + "/lena.png");

	try {
		Image img;
		ImageIO::loadPNG(img, inputFile);

		Image imgf( img.width(), img.height(), IFormat::RGBA_FLOAT );
		Image out( img.width(), img.height(), IFormat::RGBA_FLOAT );
		img.convert( imgf );

		Homography hfilter;
		IFilterVector8 H = calc_homography( 0.0f, 0.0f, 0.5f, 0.5f, 100.0f, 100.0f, 0.001f, 0.0f );
		Color black( 0.0f, 0.0f, 0.0f, 1.0f );
		hfilter.apply( out, imgf, H, black );
		ImageIO::savePNG(out, "outhomography.png");
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
