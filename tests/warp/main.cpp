#include <cvt/io/Resources.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/util/Time.h>

using namespace cvt;

int main()
{
	Resources r;
	Image in, out, warp, tmp;

	tmp.load( r.find("distort.png") );
	in.reallocate( tmp.width(), tmp.height(), IFormat::RGBA_FLOAT );
	tmp.convert( in );

	warp.reallocate( in.width() + 250, in.height() + 250, IFormat::GRAYALPHA_FLOAT );

	IWarp::warpUndistort( warp, -0.434369f, 0.194401f, 376.734f, 253.739f, 650.228f, 651.185f, in.width(), in.height(), 0, 0.00115974f, 0.00120469f );

	Time t;
	IWarp::apply( out, in, warp );
	std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;

	out.save( "warped.png" );
}
