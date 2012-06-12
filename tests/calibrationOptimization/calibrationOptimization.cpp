#include <iostream>

#include <CamCalibCostFunction.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/Vector.h>
#include <cvt/util/Time.h>

using namespace cvt;

void genGrid( std::vector<Vector3f>& pts )
{
	Vector3f p( 0.0f, 0.0f, 0.0f );
	float step = 0.1f;

	for( size_t y = 0; y < 6; y++ ){
		p.x = 0.0f;
		for( size_t x = 0; x < 6; x++ ){
			pts.push_back( p );
			p.x += step;
		}
		p.y += step;
	}
}

void synthesizeView( cvt::Matrix4f& pose )
{
	// generate a pose:    
    pose.setIdentity();
    Vector3f rnd;
    rnd.x = Math::rand( -Math::HALF_PI, Math::HALF_PI );
    rnd.y = Math::rand( -Math::HALF_PI, Math::HALF_PI );
    rnd.z = Math::rand( -Math::HALF_PI, Math::HALF_PI );
    pose.setRotationXYZ( rnd.x, rnd.y, rnd.z );
    pose[ 0 ][ 3 ] = Math::rand( 0.5f, 5.0f );
    pose[ 1 ][ 3 ] = Math::rand( 0.5f, 5.0f );
    pose[ 2 ][ 3 ] = Math::rand( 0.5f, 5.0f );
}

void generateMeasurements( std::vector<Vector2f> & measurements, const Matrix3f & K, const Matrix4f& pose, const std::vector<Vector3f> & p3d )
{
    measurements.resize( p3d.size() );
    Vector3f pcam;
    for( size_t i = 0; i < p3d.size(); i++ ){
        pcam = pose * p3d[ i ];
        pcam = K * pcam;
        measurements[ i ].x = pcam.x / pcam.z;
        measurements[ i ].y = pcam.y / pcam.z;
        // TODO: how do we distort the point?!
    }
}

int main( void )
{
    srand( time( NULL ) );
	std::vector<Vector3f> p3d;

    Matrix3f Ktrue;
    Ktrue.setIdentity();
    Ktrue[ 0 ][ 0 ] = 600.0f;
    Ktrue[ 0 ][ 2 ] = 342.6f;
    Ktrue[ 1 ][ 1 ] = 602.0f;
    Ktrue[ 1 ][ 2 ] = 292.6f;
	
	genGrid( p3d );

	CamCalibCostFunction cccf( p3d );

	size_t numViews = 10;
    while( numViews-- ){
		Matrix4f pose;
		std::vector<Vector2f> meas;
        synthesizeView( pose );
        generateMeasurements( meas, Ktrue, pose, p3d );
		cccf.add( meas, pose );
	}

    Matrix3f Kinit = Ktrue;
    Kinit[ 0 ][ 0 ] += 20.0f;
    Kinit[ 1 ][ 1 ] += 30.0f;
    Kinit[ 0 ][ 2 ] = 320.0f;
    Kinit[ 1 ][ 2 ] = 240.0f;
    cccf.setInitialK( Kinit );

    std::cout << "Initial Reprojection Error (SUM): " << cccf.reprojectionError() << std::endl;

    size_t iterations = 2;
    Time t;
    cccf.optimize( iterations );
    std::cout << "Optimization took: " << t.elapsedMilliSeconds() << "ms" << std::endl;
    std::cout << "Final Reprojection Error (SUM): " << cccf.reprojectionError() << std::endl;
	
	return 0;
}
