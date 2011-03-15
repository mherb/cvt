#include <cvt/util/PluginFile.h>
#include <cvt/util/PluginManager.h>
#include <cvt/util/String.h>

using namespace cvt;

int main()
{
	String file( "./bin/plugins/Dummy.so" );

	PluginManager::instance().loadDefault();
	try {
		PluginFile p( file.c_str() );
		p.load();
	} catch( Exception e ) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
