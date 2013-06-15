#include <cvt/vision/features/RowLookupTable.h>
#include <cvt/util/CVTTest.h>

using namespace cvt;
bool _featureSetTest()
{
	FeatureSet fset;
	Feature f;
	for( size_t i = 0; i < 5; i++ ){
			f.pt.y = 2 * i;
		for( size_t k = 0; k < 5; k++ ){
			f.pt.x = 2*k;
			fset.add( f );
		}
	}

	f.pt.y = 100;
	fset.add( f );

	RowLookupTable rlt( fset );

	bool b = true, result = true;

	b &= rlt.isValidRow( 0 );
	b &= rlt.isValidRow( 2 );
	b &= rlt.isValidRow( 4 );
	b &= rlt.isValidRow( 6 );
	b &= rlt.isValidRow( 8 );
	b &= rlt.isValidRow( 100 );
	b &= !rlt.isValidRow( 1 );
	b &= !rlt.isValidRow( 3 );
	b &= !rlt.isValidRow( 7 );
	b &= !rlt.isValidRow( 9 );
	b &= !rlt.isValidRow( 10 );
	b &= !rlt.isValidRow( -1 );
	if( !b ){
		CVTTEST_PRINT( "isValidRow()", b );
	}
	result &= b;

	b &= rlt.row( 0 ).start == 0;
	b &= rlt.row( 2 ).start == 5;
	b &= rlt.row( 4 ).start == 10;
	b &= rlt.row( 6 ).start == 15;
	b &= rlt.row( 8 ).start == 20;
	b &= rlt.row( 100 ).start == 25;
	b &= rlt.row( 0 ).len == 5;
	b &= rlt.row( 2 ).len == 5;
	b &= rlt.row( 4 ).len == 5;
	b &= rlt.row( 6 ).len == 5;
	b &= rlt.row( 8 ).len == 5;
	b &= rlt.row( 100 ).len == 1;

	if( !b ){
		CVTTEST_PRINT( "Row content", b );
	}
	result &= b;

	return result;
}

BEGIN_CVTTEST( RowLookupTable )

	bool testResult = true;
	bool b = false;

	b = _featureSetTest();
	CVTTEST_PRINT( "FeatureSet -> RowLookupTable", b );
	testResult &= b;

	return testResult;
END_CVTTEST
