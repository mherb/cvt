#include <cvt/util/List.h>
#include <iostream>

using namespace cvt;

int main()
{
	List<int> l;

	l.append( 1 );
	l.append( 2 );
	l.prepend( 0 );

	std::cout << "Size: " <<l.size() << std::endl;

	for( List<int>::Iterator it = l.begin(); it != l.end(); ++it )
		std::cout << *it << std::endl;

	std::cout << "Front: " << l.front() << std::endl;
	std::cout << "Back: " << l.back() << std::endl;

	l.remove( l.begin() );

	for( List<int>::Iterator it = l.begin(); it != l.end(); ++it )
		std::cout << *it << std::endl;


	l.clear();

	std::cout << "Size: " <<l.size() << std::endl;
	for( List<int>::Iterator it = l.begin(), end = l.end(); it != end; ++it )
		std::cout << *it << std::endl;

}
