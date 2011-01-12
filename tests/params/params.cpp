#include <iostream>

#include <cvt/util/ParamInfo.h>
#include <cvt/util/ParamSet.h>
#include <cvt/util/Exception.h>

#include <cvt/gfx/Image.h>

using namespace cvt;

void testStandard()
{
	ParamInfo* params[] =
	{
		new TypedParamInfo<int8_t>( "int8Param",  1 ),
		new TypedParamInfo<double>( "doubleParam",  1 ),
		new TypedParamInfo<double>( "rangedDouble", -10.0, 10.0, 0.0 ),
		new TypedParamInfo<Image*>( "image",  1 )
	};
		
	ParamSet pSet( params, 4, true );

	size_t i8 = pSet.paramHandle( "int8Param" );
	size_t db = pSet.paramHandle( "doubleParam" );
	size_t rdb = pSet.paramHandle( "rangedDouble" );
	size_t img = pSet.paramHandle( "image" );

	Image testImage;

	pSet.setArg<double>( db, 2.1 );
	pSet.setArg<int8_t>( i8, 2 );
	pSet.setArg( img, &testImage );
	pSet.setArg( rdb, 45.5 );

	double val = pSet.arg<double>( db );
	int8_t vali = pSet.arg<int8_t>( i8 );
	Image * iPtr = pSet.arg<Image*>( img );

	std::cout << "Double: " << val << std::endl;
	std::cout << "int8: " << (int)vali << std::endl;
	std::cout << "Image: " << iPtr << std::endl;
	val = pSet.arg<double>( rdb );
	std::cout << "Ranged double: " << val << std::endl;

	std::cout << pSet << std::endl;
}

struct MyParams
{
	int8_t i;
	double d;
	float  f;
};

void testWithStruct()
{	
	
	ParamInfo* pInfos[] =
	{
		new TypedParamInfo<int8_t>( "i", 1, offsetof( MyParams, i ) ),
		new TypedParamInfo<double>( "d", 1, offsetof( MyParams, d ) ),
		new TypedParamInfo<float>( "f", 1, offsetof( MyParams, f ) )
	};

	ParamSet pSet( pInfos, 3, false );

	size_t iHandle = pSet.paramHandle( "i" );
	size_t fHandle = pSet.paramHandle( "f" );
	size_t dHandle = pSet.paramHandle( "d" );

	pSet.setArg( iHandle, int8_t( 3 ) );
	pSet.setArg<float>( fHandle, 1.8f );
	pSet.setArg<double>( dHandle, 0.55 );


	MyParams * params;
	params = pSet.ptr<MyParams>();
	std::cout << "i: " << (int)params->i << std::endl;
	std::cout << "f: " << params->f << std::endl;
	std::cout << "d: " << params->d << std::endl;

	params->i = 13;
	params->f = 20.8f;
	params->d = 0.56453;

	std::cout << "i: " << ( int )pSet.arg<int8_t>( iHandle ) << std::endl;
	std::cout << "f: " << pSet.arg<float>( fHandle ) << std::endl;
	std::cout << "d: " << pSet.arg<double>( dHandle ) << std::endl;
}

int main( void )
{
	std::cout << "Standard Parameter Set" << std::endl;
	testStandard();
	std::cout << std::endl;

	std::cout << "Parameter Set with Structure of params" << std::endl;
	testWithStruct();
	std::cout << std::endl;

	return 0;
}