#include <cvt/util/Stack.h>

using namespace cvt;

int main()
{
	Stack<int> s( 5 );

	for( int i = 0; i < 20; i++ )
		s.push( i );

	std::cout << "OK size: " << s.size() << std::endl;

	while( !s.isEmpty() )
		std::cout << s.pop() << std::endl;
}
