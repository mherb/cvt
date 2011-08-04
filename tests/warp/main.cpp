#include <cvt/io/Resources.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/IWarp.h>

using namespace cvt;

int main()
{
	Resources r;
	Image in, out, warp, tmp;

	tmp.load( r.find("lena.png") );
	in.reallocate( tmp.width(), tmp.height(), IFormat::RGBA_UINT8 );
	tmp.convert( in );

	warp.reallocate( in.width(), in.height(), IFormat::GRAYALPHA_FLOAT );

	IWarp::warpFishEye( warp, 3.0f, in.width() * 0.5f, in.height() * 0.5f );
	IWarp::apply( out, in, warp );

	out.save( "warped.png" );
}
