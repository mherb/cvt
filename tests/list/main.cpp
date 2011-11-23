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

	List<int>::Iterator it= l.find( 2 );
	if( it != l.end() )
		std::cout << "Found " << *it << std::endl;
	else
		std::cout << "Not found" << std::endl;

	it= l.find( 4 );
	if( it != l.end() )
		std::cout << "Found " << *it << std::endl;
	else
		std::cout << "Not found" << std::endl;


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
