#include <cvt/gfx/ColorspaceXYZ.h>

using namespace cvt;

int main()
{
	std::cout << ColorspaceXYZ::sRGB.matrixRGBtoXYZ() << std::endl;
}
