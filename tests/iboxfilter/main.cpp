#include <cvt/gfx/Image.h>
#include <cvt/util/Time.h>
#include <cvt/util/String.h>
#include <cvt/io/Resources.h>

#include <cvt/gfx/IBoxFilter.h>

using namespace cvt;

int main()
{
	Resources resources;
	Image tmp( resources.find( "lena_g.png" ) );
	Image lena;
	tmp.convert( lena, IFormat::GRAY_FLOAT );
	Image dst;

	Time t;
	IBoxFilter::boxfilter( lena, lena, 2, 2 );
	std::cout << t.elapsedMilliSeconds() << std::endl;
	lena.save( "boxfilter.png" );

//	lena.save( "boxfilter0.png" );

	return 0;
}
