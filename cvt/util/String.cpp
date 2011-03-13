#include <cvt/util/CVTTest.h>
#include <cvt/util/String.h>
#include <iostream>
#include <stdio.h>

namespace cvt {

	BEGIN_CVTTEST( string )
		String str( "Hallo Welt" );
		std::cout << str << std::endl;

		size_t len = str.length();
		for( size_t i = 0; i <= len; i++ )
			printf("0x%0X ", str[ i ] );
		printf("\n");

		std::cout << "Length: " << str.length() << std::endl;


		String substr( str, 0, 5 );
		std::cout << substr << std::endl;

		String substr2( str, 6, 4 );
		std::cout << substr2 << std::endl;
		std::cout << "== : " << ( substr2 == "Welt" ) << std::endl;


		substr += " blub";
		std::cout << substr << std::endl;

		std::cout << "Length: " << substr.length() << std::endl;

		std::cout << "Prefix: " << str.hasPrefix( "Hallo" ) << std::endl;
		std::cout << "Prefix: " << substr.hasPrefix( "Hallo" ) << std::endl;

		std::cout << "Suffix: " << str.hasSuffix( "Welt" ) << std::endl;
		std::cout << "Suffix: " << substr.hasSuffix( "blub" ) << std::endl;

		return true;
	END_CVTTEST

}
