#include <cvt/gfx/Image.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/Time.h>

#include "BoxFilter.h"

using namespace cvt;

static inline void boxfun( Image& dst, const Image& iimg, size_t r )
{
	dst.reallocate( iimg );
	dst.fill( Color::WHITE );

	cvt::IMapScoped<float> mapdst( dst );
	cvt::IMapScoped<const float> mapsrc( iimg );

	SIMD::instance()->boxFilterPrefixSum1f( mapdst.ptr(), mapdst.stride(), mapsrc.ptr(), mapsrc.stride(), dst.width(), dst.height(), 2*r+1, 2*r+1 );
}

int main( int argc, char** argv )
{
	Image tmp( argv[ 1 ] );
	Image src, dst, isrc;

	tmp.convert( src, IFormat::GRAY_FLOAT );
	src.integralImage( isrc );
	Time t;
	BoxFilter( dst, isrc, Vector2i( 7, 7 ) );
	std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;
	dst.save( "outbox1.png" );

	t.reset();
	boxfun( dst, isrc, 3 );
	std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;
	dst.save( "outbox2.png" );



}
