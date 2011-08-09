#include <cvt/vision/CameraCalibration.h>
#include <cvt/util/CVTTest.h>

#include "cvt/io/xml/XMLDocument.h"

namespace cvt {

BEGIN_CVTTEST( CameraCalibration )

bool result = true;
bool b;

CameraCalibration cc;

b = ( cc.hasExtrinsics() == false );

Matrix4f ext;
ext.setIdentity();
ext.setRotationXYZ( 0.4f, 0.2f, -2.7f );
ext.setTranslation( 10.5f, 20.6f, -342.7f );
cc.setExtrinsics( ext );

b &= ( cc.hasExtrinsics() == true );
result &= b;
CVTTEST_PRINT( "hasExtrinsics()", b );

b = ( cc.hasIntrinsics() == false );
Matrix3f K( 245.4f, 0.4f, 233.f, 0.0f, 233.0f, 342.0f, 0.0f, 0.0f, 1.0f );
cc.setIntrinsics( K );
b &= ( cc.hasIntrinsics() == true );
result &= b;
CVTTEST_PRINT( "hasIntrinsics()", b );

b = ( cc.hasDistortion() == false );
CVTTEST_PRINT( "hasDistortion()", b );
result &= b;

b = ( ext == cc.extrinsics() );
CVTTEST_PRINT( "setExtrinsics()", b );
result &= b;

b = ( K == cc.intrinsics() );
CVTTEST_PRINT( "setIntrinsics()", b );
result &= b;

XMLDocument xmlDoc;
xmlDoc.addNode( cc.serialize() );
xmlDoc.save( "camcalibtest.xml" );

XMLDocument doc;
doc.load( "camcalibtest.xml" );

XMLNode * node = doc.nodeByName( "CameraCalibration" );

CameraCalibration cc2;
cc2.deserialize( node );

b  = ( cc.intrinsics() == cc2.intrinsics() );
b &= ( cc.extrinsics() == cc2.extrinsics() );

//b &= ( cc.radialDistortion() == cc2.radialDistortion() );
//b &= ( cc.tangentialDistortion() == cc2.tangentialDistortion() );

CVTTEST_PRINT( "serialize/deserialize", b );



return result;

END_CVTTEST

}