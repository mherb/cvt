#include <cvt/util/Stack.h>

using namespace cvt;

int main()
{
	Stack<int> s( 5 );

	for( int i = 0; i < 25; i++ )
		s.push( i );

	std::cout << "Size: " << s.size() << std::endl;
	std::cout << "Empty: " << s.isEmpty() << std::endl;

	while( !s.isEmpty() )
		std::cout << s.pop() << std::endl;
}
