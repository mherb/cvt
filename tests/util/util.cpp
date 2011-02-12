#include <stdio.h>
#include <cvt/util/Util.h>

using namespace cvt;

int main()
{
	printf("0x%X\n", Util::bswap16( 0x1122 ) );
	printf("0x%X\n", Util::bswap32( 0x11223344 ) );
	printf("0x%llX\n", Util::bswap64( 0x1122334455667788 ) );
}
