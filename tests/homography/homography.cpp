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

/*static inline void xmatrix_set_rotx(XMatrix* mat, double angle)
{
    double rad = -angle/180.0*M_PI;
    double c = cos(rad);
    double s = sin(rad);

    mat->m[0][0] = 1.0;
    mat->m[0][1] = 0.0;
    mat->m[0][2] = 0.0;
    mat->m[1][0] = 0.0;
    mat->m[1][1] = 1.0/c;
    mat->m[1][2] = 0.0;
    mat->m[2][0] = 0.0;
    mat->m[2][1] = -s/(c*1024.0);
    mat->m[2][2] = 1.0;
}

static inline void xmatrix_set_roty(XMatrix* mat, double angle)
{
    double rad = -angle/180.0*M_PI;
    double c = cos(rad);
    double s = sin(rad);

    mat->m[0][0] = 1.0/c;
    mat->m[0][1] = 0.0;
    mat->m[0][2] = 0.0;
    mat->m[1][0] = 0.0;
    mat->m[1][1] = 1.0;
    mat->m[1][2] = 0.0;
    mat->m[2][0] = -s/(1024.0*c);
    mat->m[2][1] = 0.0;
    mat->m[2][2] = 1.0;
}*/

static inline void RotY( Matrix3f& mat, float angle )
{
    float rad = Math::deg2Rad( angle );
    float c = Math::cos(rad);
    float s = Math::sin(rad);

    mat[0][0] = 1.0f/c;
    mat[0][1] = 0.0f;
    mat[0][2] = 0.0f;
    mat[1][0] = 0.0f;
    mat[1][1] = 1.0f;
    mat[1][2] = 0.0f;
    mat[2][0] = -s/( 4096.0f * c);
    mat[2][1] = 0.0f;
    mat[2][2] = 1.0f;
}


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

        cvt::Matrix3f H, S, T;
		T.setIdentity();
		T[ 0 ][ 2 ] = -img.width() / 2.0f;
		T[ 1 ][ 2 ] = -img.height() / 2.0f;

		S.setIdentity();
		S[ 0 ][ 0 ] = 0.5f;
		S[ 1 ][ 1 ] = 0.5f;

		RotY( H, 30.0f );
		H = T.inverse() * S * H * T;

//		H.setHomography( Math::deg2Rad( 23.0f ), 0.0f, 1.5f, 0.8f, 10.0f, 10.0f, 0.0f, 0.0f );
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
