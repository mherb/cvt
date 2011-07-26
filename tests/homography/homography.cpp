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
    mat[2][0] = -s/( 1024.0f * c);
    mat[2][1] = 0.0f;
    mat[2][2] = 1.0f;
}

static inline void RotY2( Matrix3f& mat, float angle, float fx, float fy, float d )
{
    float rad = Math::deg2Rad( angle );
    float c = Math::cos(rad);
    float s = Math::sin(rad);

    mat[0][0] = c * fx;
    mat[0][1] = 0.0f;
    mat[0][2] = 0;
    mat[1][0] = 0.0f;
    mat[1][1] = fy;
    mat[1][2] = 0.0f;
    mat[2][0] = -s;
    mat[2][1] = 0.0f;
    mat[2][2] = d;
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

		for( float angle = 0; angle < 360; angle += 5.0f ) {
			cvt::Matrix3f H, S, T;
			T.setIdentity();
			T[ 0 ][ 2 ] = -img.width() / 2.0f;
			T[ 1 ][ 2 ] = -img.height() / 2.0f;

			//S.setIdentity();
			//S[ 0 ][ 0 ] = 0.5f;
			//S[ 1 ][ 1 ] = 0.5f;

			RotY2( H, angle, 1024.0f, 1024.0f, 1024.0f );
			H = T.inverse() * H * T;

			//std::cout << H << std::endl;

			//		H.setRotationY( Math::deg2Rad( 30.0f ) );
			//		Matrix3f K( 1.0f, 0.0f, img.width() / 2.0f,
			//				       0.0f, 1.0f, img.height() / 2.0f,
			//					   0.0f, 0.0f, 1.0f );
			//		Matrix3f Kinv( 1.0f, 0.0f, -img.width() / 2.0f,
			//				       0.0f, 1.0f, -img.height() / 2.0f,
			//					   0.0f, 0.0f, -100.0f );
			//
			//		H = K * H * Kinv;

			//std::cout << H << std::endl;

			String path;
			path.sprintf("outhomographyrot%03d.png", ( int ) angle );
	
			std::cout << path << std::endl;

			//		H.setHomography( Math::deg2Rad( 23.0f ), 0.0f, 1.5f, 0.8f, 10.0f, 10.0f, 0.0f, 0.0f );
			if( !H.inverseSelf() ){
				std::cerr << "Could not invert" << std::endl;
				continue;
			}


			Color black( 0.0f, 0.0f, 0.0f, 1.0f );
			Time t;
	//	t.reset();
	//	hfilter.apply( out, imgf, H, black );
	//	std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;
		//out.save( "outhomography.png" );
	//		out.save( path );

			// otherwise denormalized float stuff kicks in
			out2.fill( Color::BLACK );
			//t.reset();
			ITransform::apply( out2, imgf, H );
			//std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;
			out2.save( path );
		//	out2.save( "outhomography2.png" );
		}
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
