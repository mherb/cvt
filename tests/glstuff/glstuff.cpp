#include <cvt/gui/Application.h>
#include <cvt/gl/GLFormat.h>

#include <iostream>

using namespace cvt;

int main()
{
	GLFormat glformat;
	glformat.defaultValues();
	std::cout << glformat << std::endl;
}
