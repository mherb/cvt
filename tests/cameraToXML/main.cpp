#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/io/FileSystem.h>
#include <cvt/util/Data.h>
#include <cvt/util/DataIterator.h>
#include <cvt/util/String.h>
#include <cvt/vision/CameraCalibration.h>

using namespace cvt;

int main( int argc, char** argv )
{
	float scale = 1.0f;

	if( argc != 3 && argc != 4 )
		return 0;

	if( argc == 4 ) {
		String str( argv[ 3 ] );
		scale = str.toFloat();
	}

	if( FileSystem::exists( argv[ 1 ] ) ){
		Data data;
		String str, tmp;
		Matrix3f K, R;
		Vector3f t;
		CameraCalibration calib;

		FileSystem::load( data, argv[ 1 ] );
		DataIterator it( data );


		for( int i = 0; i < 3; i++ ) {
			if( it.nextLine( tmp ) ) {
				str += tmp;
			} else
				return 0;
		}
		K = Matrix3f::fromString( str );
		K *= scale;
		K[ 2 ][ 2 ] = 1.0f;
		std::cout << K << std::endl;


		it.nextLine( tmp );

		str = "";
		for( int i = 0; i < 3; i++ ) {
			if( it.nextLine( tmp ) ) {
				str += tmp;
			} else
				return 0;
		}
		R = Matrix3f::fromString( str );
		std::cout << R << std::endl;

		if( !it.nextLine( str ) )
			return 0;

		t = Vector3f::fromString( str );
		std::cout << t << std::endl;

		calib.setIntrinsics( K );

		Matrix4f E;
		E = R.transpose().toMatrix4();
		Vector3f tt = -R.transpose() * t;
		E[ 0 ][ 3 ] = tt[ 0 ];
		E[ 1 ][ 3 ] = tt[ 1 ];
		E[ 2 ][ 3 ] = tt[ 2 ];
		calib.setExtrinsics( E );

		calib.save( argv[ 2 ] );
	}

}
