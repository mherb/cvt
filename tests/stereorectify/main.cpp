#include <iostream>

#include <cvt/io/Resources.h>
#include <cvt/io/xml/XMLDocument.h>
#include <cvt/vision/StereoCameraCalibration.h>
#include <cvt/vision/Vision.h>

#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/util/Delegate.h>
#include <cvt/math/Quaternion.h>

using namespace cvt;
void loadCameraCalib( CameraCalibration& camCalib, const String& file )
{
	XMLDocument xmlDoc;
	xmlDoc.load( file );
	
	XMLNode* node = xmlDoc.nodeByName( "CameraCalibration" );
	camCalib.deserialize( node );
}

struct UndistortOp
{
	UndistortOp( const CameraCalibration& calib ) : _calib( calib ) {}

	inline Vector2f operator()( const Vector2f& pt ) const { return _calib.undistortPoint( pt - Vector2f(100, 100 ) ) + Vector2f( 0, 0 ); }

	CameraCalibration _calib;
};


int main( int argc, char** argv )
{
	// FIXME: add argc/argv check
	CameraCalibration left;
	CameraCalibration right;

	StereoCameraCalibration( left, right );

	loadCameraCalib( left, argv[ 1 ] );
	loadCameraCalib( right, argv[ 2 ] );

	Image input( argv[ 3 ] );
	Image warp( input.width() + 200, input.height() + 200, IFormat::GRAYALPHA_FLOAT );
	Image  out( input.width() + 200, input.height() + 200, input.format() );

	UndistortOp op( left );
	IWarp::warpGeneric( warp, op );
//	IWarp::warpGeneric( warp, Delegate<Vector2f ( const Vector2f& )>( &left, &CameraCalibration::undistortPoint ) );
	IWarp::apply( out, input, warp );

	{
		IMapScoped<uint8_t> map( out );
		uint8_t* ptr = map.ptr();
		size_t stride = map.stride();
		for( float x = 0; x < input.width(); x++ ) {
			Vector2f pt = left.inverseUndistortPoint2( Vector2f( x, 0  ) ) + Vector2f( 100, 100 );
			int px = ( int ) ( pt.x + 0.5f );
			int py = ( int ) ( pt.y + 0.5f );
			if( px >= 0 && px < out.width() && py >= 0 && py < out.height() )
				*( ( uint32_t* )( ptr + stride * py + px * 4 ) ) = 0xffffffff;
		}

		for( float x = 0; x < input.width(); x++ ) {
			Vector2f pt = left.inverseUndistortPoint2( Vector2f( x, input.height() - 1 ) ) + Vector2f( 100, 100 );
			int px = ( int ) ( pt.x + 0.5f );
			int py = ( int ) ( pt.y + 0.5f );
			if( px >= 0 && px < out.width() && py >= 0 && py < out.height() )
				*( ( uint32_t* )( ptr + stride * py + px * 4 ) ) = 0xffffffff;
		}

	}

	out.save("output.png");
}
