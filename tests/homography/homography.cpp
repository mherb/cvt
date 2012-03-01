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

class WarpTx : public Function<Vector2f, Vector2f>
{
	public:
		WarpTx( float tx ) : _tx( tx )
		{
		}

		~WarpTx()
		{
		}

		Vector2f operator()( const Vector2f& v ) const
		{
			//Matrix3f m;
			//m.setAffine( Math::deg2Rad( 45.0f ), Math::deg2Rad( 10.0f ), 2.0f, 2.0f, 0.0f, -200.0f );				
			//return m * v;
			Vector2f p = v;
			p.y = v.y + 100.0f * Math::sin( 2.0 * Math::TWO_PI * v.x / 512.0f );
			return p;
		}

	private:
		float _tx;
};

int main()
{
	std::string dataFolder( DATA_FOLDER );
	std::string inputFile( dataFolder + "/lena.png" );

	try {
		Image img;
		img.load( inputFile.c_str() );

		WarpTx w( 5.0f );
		Image outg( img.width(), img.height(), IFormat::RGBA_UINT8 );
		ITransform::apply( outg, img, w );
		outg.save( "wtx.png" );
		return 0;

		Image imgf( img.width(), img.height(), IFormat::RGBA_FLOAT );
		Image out( img.width(), img.height(), IFormat::RGBA_FLOAT );
		Image out2( img.width(), img.height(), IFormat::RGBA_FLOAT );
		img.convert( imgf );

		Homography hfilter;

		for( float angle = 0; angle < 360; angle += 5.0f ) {
			cvt::Matrix3f H, S, T;
			T.setIdentity();
			T[ 0 ][ 2 ] = -( img.width() - 1.0f ) / 2.0f;
			T[ 1 ][ 2 ] = -( img.height() - 1.0f ) / 2.0f;

			RotY2( H, angle, 1024.0f, 1024.0f, 1024.0f );
			H = T.inverse() * H * T;

			//std::cout << H << std::endl;

			//		H.setHomography( Math::deg2Rad( 23.0f ), 0.0f, 1.5f, 0.8f, 10.0f, 10.0f, 0.0f, 0.0f );
			/*if( !H.inverseSelf() ){
				std::cerr << "Could not invert" << std::endl;
				continue;
			}*/

			String path;
			Time t;
#if 0
			path.sprintf("outhomographyrotOTHER%03d.png", ( int ) angle );
			Color black( 0.0f, 0.0f, 0.0f, 1.0f );
			t.reset();
			hfilter.apply( out, imgf, H.inverse(), black );
			std::cout << "OLD " << t.elapsedMilliSeconds() << " ms" << std::endl;
			//out.save( path );
#endif

			path.sprintf("outhomographyrot%03d.png", ( int ) angle );
			// otherwise denormalized float stuff kicks in
			out2.fill( Color::BLACK );
			t.reset();
			ITransform::apply( out2, imgf, H );
			std::cout << "NEW " << t.elapsedMilliSeconds() << " ms" << std::endl;
			//out2.save( path );
		}
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
