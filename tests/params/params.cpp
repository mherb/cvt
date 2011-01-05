#include <iostream>

#include <cvt/util/ParamInfo.h>
#include <cvt/util/ParamSet.h>
#include <cvt/util/Exception.h>

using namespace cvt;

int main( void )
{
	ParamInfo params[] = 
	{
		ParamInfo( PTYPE_INT8, "int8Param",  1 ),
		ParamInfo( PTYPE_DOUBLE, "doubleParam",  1 )
	};
	
	ParamSet pSet( params, 2, true );
	
	size_t i8 = pSet.paramHandle( "int8Param" );
	size_t db = pSet.paramHandle( "doubleParam" );		

	pSet.setArg<double>( db, 2.0 );
	pSet.setArg<int8_t>( i8, 2 );
	
	double val = pSet.arg<double>( db );
	int8_t vali = pSet.arg<int8_t>( i8 );
	
	std::cout << "Double: " << val << std::endl;
	std::cout << "int8: " << (int)vali << std::endl;
	
	std::cout << pSet << std::endl;		

	return 0;
}