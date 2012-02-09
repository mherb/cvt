#include <cvt/gfx/IMapScoped.h>

using namespace cvt;

int main()
{
	Image i;

	IMapScoped<uint8_t> map( i );
}
