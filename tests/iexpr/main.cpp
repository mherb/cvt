#include <cvt/gfx/IExpr.h>

using namespace cvt;

int main()
{
	Image bla, dummy;

	std::cout << -bla * 10.0f + 2.0f + bla * 3 -5.0f + dummy * 25 + -10.0f << std::endl;

	return 0;
}
