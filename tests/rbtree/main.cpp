#include <cvt/container/RBTree.h>
#include <iostream>

using namespace cvt;

int main()
{
	RBTree<int> tree;

	tree.insert( 5 );
	tree.insert( 1 );
	tree.insert( 3 );

	for( RBTree<int>::Iterator it = tree.begin(); it != tree.end(); ++it )
		std::cout <<  *it << std::endl;
	std::cout << "-------" << std::endl;

	RBTree<int>::Iterator it = tree.insert( 4 );

	for( RBTree<int>::Iterator it = tree.begin(); it != tree.end(); ++it )
		std::cout <<  *it << std::endl;

	std::cout << "-------" << std::endl;

	std::cout << tree.front() << std::endl;
	std::cout << tree.back() << std::endl;

	std::cout << "-------" << std::endl;

	tree.remove( it );

	for( RBTree<int>::Iterator it = tree.begin(); it != tree.end(); ++it )
		std::cout <<  *it << std::endl;

	it = tree.find( 0 );
	if( it == tree.end() )
		std::cout << "OK" << std::endl;


	it = tree.find( 1 );
	tree.remove( it );

	std::cout << "-------" << std::endl;
	for( RBTree<int>::Iterator it = tree.begin(); it != tree.end(); ++it )
		std::cout <<  *it << std::endl;


	tree.clear();
	std::cout << "-------" << std::endl;
	for( RBTree<int>::Iterator it = tree.begin(); it != tree.end(); ++it )
		std::cout <<  *it << std::endl;

}
