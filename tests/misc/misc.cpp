#include <cvt/math/Math.h>

using namespace cvt;

int main()
{
    float x = Math::PI / 2.0f;
    double d = x;

    Math::Cos( x );
    Math::Cos( d );
}
