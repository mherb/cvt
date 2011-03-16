#include <cvt/util/PluginFile.h>
#include <cvt/util/PluginManager.h>
#include <cvt/util/String.h>

using namespace cvt;

int main()
{
#ifndef APPLE
	String file( "./bin/plugins/Dummy.so" );
#else
	String file( "./bin/plugins/Dummy.dylib" );
#endif

	PluginManager::instance().loadDefault();
	try {
		PluginFile p( file.c_str() );
		p.load();
	} catch( Exception e ) {
		std::cout << e.what() << std::endl;
	}

	size_t n = PluginManager::instance().getIFilterSize();
	for( size_t i = 0; i < n; i++ )
		std::cout << *( PluginManager::instance().getIFilter( i ) ) << std::endl;

	return 0;
}
