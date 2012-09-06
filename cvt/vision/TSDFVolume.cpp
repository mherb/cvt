#include <cvt/vision/TSDFVolume.h>
#include <cvt/cl/kernel/TSDFVolume/TSDFVolume.h>

namespace cvt
{

	TSDFVolume::TSDFVolume( const Matrix4f& gridtoworld, size_t width, size_t height, size_t depth, float truncation = 5.0f ) :
		_width( width ),
		_height( height ),
		_depth( depth ),
		_trunc( truncation ),
		_clvolume( sizeof( cl_float2 ) * width * height * depth ),
		_clTG2W( ( void*) gridtoworld.ptr(), sizeof( float ) * 12 ),
		_clvolclear( _TSDFVolume_source, "TSDFVolume_clear" ),
		_clvoladd( _TSDFVolume_source, "TSDFVolume_add" )
	{

	}

}
