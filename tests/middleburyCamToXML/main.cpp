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

	if( argc != 3 ) {
		std::cout << "usage: " << argv[ 0 ] << " parameters scale" << std::endl;
		return 0;
	}

	String str( argv[ 2 ] );
	scale = str.toFloat();

	if( FileSystem::exists( argv[ 1 ] ) ){
		String token;
		Data data;
		size_t numentries;

		FileSystem::load( data, argv[ 1 ] );
		DataIterator it( data );

		numentries = it.nextLong();

		for( size_t i = 0; i < numentries; i++ ) {

			Data data;
			String file;
			Matrix3f K, R;
			Vector3f t;
			CameraCalibration calib;

			it.nextToken( file, " \n\t\r" );
			file += ".xml";

			for( int y = 0; y < 3; y++ ) {
				for( int x = 0; x < 3; x++ ) {
					K[ y ][ x ] = it.nextDouble();
				}
			}
			K *= scale;
			K[ 2 ][ 2 ] = 1.0f;

			std::cout << K << std::endl;

			calib.setIntrinsics( K );

			for( int y = 0; y < 3; y++ ) {
				for( int x = 0; x < 3; x++ ) {
					R[ y ][ x ] = it.nextDouble();
				}
			}

			t[ 0 ] = it.nextDouble();
			t[ 1 ] = it.nextDouble();
			t[ 2 ] = it.nextDouble();


			std::cout << R << " " << t << std::endl;


			Matrix4f E;
			E = R.toMatrix4();
			E[ 0 ][ 3 ] = t[ 0 ];
			E[ 1 ][ 3 ] = t[ 1 ];
			E[ 2 ][ 3 ] = t[ 2 ];
			calib.setExtrinsics( E );

			calib.save( file );
		}
	}

}
