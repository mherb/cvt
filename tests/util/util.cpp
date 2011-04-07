#include <stdio.h>
#include <cvt/util/Util.h>

using namespace cvt;

int main()
{
	char test[] = {0xE2,0x82,0xAC,0x0};
	char test2[] ={0xF0,0x9D,0x84,0x9E,0x0};
	char* end;
	printf("0x%X\n", Util::bswap16( 0x1122 ) );
	printf("0x%X\n", Util::bswap32( 0x11223344 ) );
	printf("0x%llX\n", Util::bswap64( 0x1122334455667788 ) );
	printf("0x%X\n", Util::UTF8toUTF32( test, &end ) );
	printf("%d\n", end - test );
	printf("%p %p 0x%X 0x%X\n", end, test + 3, *end, test[ 3 ] );
	printf("0x%X\n", Util::UTF8toUTF32( test2, &end ) );
	printf("%d\n", end - test2 );
	printf("%p %p 0x%X 0x%X\n", end, test2 + 4, *end, test[ 4 ] );

}
