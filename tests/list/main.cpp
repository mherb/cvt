#include <cvt/container/List.h>
#include <cvt/math/Math.h>
#include <iostream>

using namespace cvt;

int main()
{
	List<int> l;

//	for( int i = 0; i < 100; i++ )
//		l.append( Math::rand( 0, 10000 ) );
	l.append( 1 );
	l.append( 2 );
	l.prepend( 0 );

	for( List<int>::Iterator it = l.begin(); it != l.end(); ++it )
		std::cout << *it << std::endl;

	std::cout << "\n\n" << std::endl;

	l.sort();

	for( List<int>::Iterator it = l.begin(); it != l.end(); ++it )
		std::cout << *it << std::endl;

return 0;
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
