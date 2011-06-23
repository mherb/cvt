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

static cvt::Matrix3f calc_homography( float theta, float phi, float sx, float sy, float tx, float ty, float v1, float v2 )
{
	Eigen::Matrix3f h, ih;
	Eigen::Transform<float,2> affine;
	
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
	
	ih = h.inverse();	
//	ih /= powf( ih.determinant(), 1.0f / 3.0f );
	
	ih.transposeInPlace();	
	return *( ( cvt::Matrix3f* )ih.data() );	
}

int main()
{
	std::string dataFolder(DATA_FOLDER);
	std::string inputFile(dataFolder + "/lena.png");

	try {
		Image img;
		img.load( inputFile.c_str() );

		Image imgf( img.width(), img.height(), IFormat::GRAY_FLOAT );
		Image out( img.width(), img.height(), IFormat::GRAY_FLOAT );
		Image out2( img.width(), img.height(), IFormat::GRAY_FLOAT );
		img.convert( imgf );

		Homography hfilter;
		cvt::Matrix3f H = calc_homography( 0.0f, 0.0f, 0.5f, 0.5f, 100.0f, 100.0f, 0.0001f, 0.0f );
//		cvt::Matrix3f H = calc_homography( 0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f );
		std::cout << "H: \n" <<  H << std::endl;
		
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
