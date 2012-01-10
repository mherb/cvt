#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include <cvt/io/Resources.h>

#include <cvt/util/Exception.h>
#include <cvt/util/Time.h>
#include <cvt/util/String.h>
#include <cvt/util/Data.h>
#include <cvt/util/DataIterator.h>
#include <cvt/io/FileSystem.h>
#include <cvt/io/xml/XMLDocument.h>
#include <cvt/vision/SparseBundleAdjustment.h>

#include <cvt/vision/slam/SlamMap.h>
#include <cvt/vision/CameraCalibration.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace cvt;


void testJointMeasures( const SlamMap & map )
{

	JointMeasurements jm;
	jm.resize( map.numKeyframes() );

	for( size_t i = 0; i < map.numFeatures(); i++ ){
		// get the MapFeature: 
		const MapFeature & feature = map.featureForId( i );

		// for all point tracks (meas. in the cams) of this point
		MapFeature::ConstPointTrackIterator camIterCurr = feature.pointTrackBegin();
		const MapFeature::ConstPointTrackIterator camIterEnd  = feature.pointTrackEnd();

		while( camIterCurr != camIterEnd ){
			MapFeature::ConstPointTrackIterator camIter = camIterCurr;
			camIter++;
			while( camIter != camIterEnd ) {
				jm.addMeasurementForEntity( *camIterCurr, *camIter, i );
				++camIter;
			}
			++camIterCurr;
		}
	}
}

void parseIntrinsics( CameraCalibration & calib, const String & calibFile )
{
	Data d;
	FileSystem::load( d, calibFile );
	DataIterator iter( d );

	Matrix3f K;
	String line;
	std::vector<String> lineTokens;
	for( size_t i = 0; i < 3; i++ ){
		if( !iter.nextLine( line ) )
			throw CVTException( "Could not parse Calibration File" );
	
		line.tokenize( lineTokens, ' ' );
		if( lineTokens.size() < 3 )
			throw CVTException( "Could not parse Calibration File" );

		K[ i ][ 0 ] = lineTokens[ 0 ].toFloat();
		K[ i ][ 1 ] = lineTokens[ 1 ].toFloat();
		K[ i ][ 2 ] = lineTokens[ 2 ].toFloat();
		lineTokens.clear();
	}
	
	calib.setIntrinsics( K );
}

void parseCamFile( SlamMap & map, const String & camFile )
{
	Data d;
	FileSystem::load( d, camFile );
	DataIterator iter( d );

	Eigen::Quaterniond q;
	Eigen::Vector3d	   t;
	Eigen::Matrix4d	   pose( Eigen::Matrix4d::Identity() );

	String line;
	std::vector<String> lineTokens;
	size_t lineNum = 0;
	std::cout << "Adding keyframes: ";
	size_t numKf = 0;	
	while( iter.nextLine( line ) ){
		lineNum++;

		if( line[ 0 ] == '#' ){
			// comment line
			continue;
		}

		line.tokenize( lineTokens, ' ' );
		if( lineTokens.size() < 7 ){
			std::cerr << "Camera File Error in line " << lineNum << std::endl;
			std::cerr << "Skipping line " << std::endl;
			continue;
		}

		q.w() = lineTokens[ 0 ].toDouble();
		q.x() = lineTokens[ 1 ].toDouble();
		q.y() = lineTokens[ 2 ].toDouble();
		q.z() = lineTokens[ 3 ].toDouble();
		t.x() = lineTokens[ 4 ].toDouble();
		t.y() = lineTokens[ 5 ].toDouble();
		t.z() = lineTokens[ 6 ].toDouble();

		pose.block<3, 3>( 0, 0 ) = q.toRotationMatrix();
		pose.block<3, 1>( 0, 3 ) = t;
		numKf = map.addKeyframe( pose );

		lineTokens.clear();
	}
	std::cout << numKf+1 << std::endl;
}

void parsePointFile( SlamMap & map, const String & file )
{
	Data d;
	FileSystem::load( d, file );
	DataIterator iter( d );

	MapFeature point( Eigen::Vector4d( 0, 0, 0, 1 ), 
					  Eigen::Matrix4d::Identity() );
	int numMeas;
	MapMeasurement meas;

	String line;
	std::vector<String> lineTokens;
	size_t lineNum = 0;
	std::cout << "Adding Points: ";
	size_t numPts = 0;	
	while( iter.nextLine( line ) ){
		lineNum++;
		if( line[ 0 ] == '#' ){
			// comment line
			continue;
		}

		line.tokenize( lineTokens, ' ' );
		if( lineTokens.size() < 7 ){
			std::cerr << "Point File Error in line " << lineNum << std::endl;
			std::cerr << "Skipping line " << std::endl;
			continue;
		}
		
		size_t tokIter = 0;
		point.estimate().x() = lineTokens[ tokIter++ ].toDouble();
		point.estimate().y() = lineTokens[ tokIter++ ].toDouble();
		point.estimate().z() = lineTokens[ tokIter++ ].toDouble();
		numMeas	  = lineTokens[ tokIter++ ].toInteger();

		numPts++;

		if( lineTokens.size() != (size_t)( 4 + numMeas * 3 ) ){
			String message( "Point file error in line: " );
			message += lineNum;
			throw CVTException( message.c_str() );
		}
		
		// add this point to the map
		size_t pointId = map.addFeature( point );
		for( int i = 0; i < numMeas; i++ ){
			size_t keyframeId = ( size_t )lineTokens[ tokIter++ ].toInteger();
			meas.point[ 0 ] = lineTokens[ tokIter++ ].toDouble();
			meas.point[ 1 ] = lineTokens[ tokIter++ ].toDouble();
			map.addMeasurement( pointId, keyframeId, meas );
		}

		lineTokens.clear();
	}
	std::cout << numPts << std::endl;
}

void parseGtPointFile( SlamMap & map, const String & file )
{
	Data d;
	FileSystem::load( d, file );
	DataIterator iter( d );

	MapFeature point( Eigen::Vector4d( 0, 0, 0, 1 ), 
					  Eigen::Matrix4d::Identity() );
	MapMeasurement meas;

	String line;
	std::vector<String> lineTokens;
	size_t lineNum = 0;
	std::cout << "Adding GT Points: ";
	size_t numPts = 0;	
	while( iter.nextLine( line ) ){
		lineNum++;
		if( line[ 0 ] == '#' ){
			// comment line
			continue;
		}

		line.tokenize( lineTokens, ' ' );
		if( lineTokens.size() < 3 ){
			std::cerr << "Point File Error in line " << lineNum << std::endl;
			std::cerr << "Skipping line " << std::endl;
			continue;
		}
		
		size_t tokIter = 0;
		point.estimate().x() = lineTokens[ tokIter++ ].toDouble();
		point.estimate().y() = lineTokens[ tokIter++ ].toDouble();
		point.estimate().z() = lineTokens[ tokIter++ ].toDouble();

		numPts++;

		// add this point to the map
		map.addFeature( point );
		lineTokens.clear();
	}
	std::cout << numPts << std::endl;
}

void parseLourakisData( SlamMap & slamMap, 
						SlamMap & gtMap,
					    const String & calibFile, 
						const String & camFile, 
						const String & ptsFile,
					    const String & gtCamFile,
						const String & gtPtFile )
{
	CameraCalibration camCalib;
	parseIntrinsics( camCalib, calibFile );

	const Matrix3f  Kf = camCalib.intrinsics();
	Eigen::Matrix3d K;
	for( size_t r = 0; r < 3; r++ )
		for( size_t c = 0; c < 3; c++ )
			K( r, c ) = Kf[ r ][ c ];
	slamMap.setIntrinsics( K );
	gtMap.setIntrinsics( K );

	parseCamFile( slamMap, camFile );
	parsePointFile( slamMap, ptsFile );

	parseCamFile( gtMap, gtCamFile );
	parseGtPointFile( gtMap, gtPtFile );
}

void compareMaps( const SlamMap & groundTruth, const SlamMap & optimized )
{

	// go over all points and compare them
	Eigen::Vector4d err;
	double errLen, errSum;
	errSum = 0.0;
	for( size_t i = 0; i < groundTruth.numFeatures(); i++ ){
		const Eigen::Vector4d & gt  = groundTruth.featureForId( i ).estimate();	
		const Eigen::Vector4d & est = optimized.featureForId( i ).estimate();
		err = gt - est;
	//	std::cout << "Point " << i;
	//    std::cout << ":\n\tGT -> [ " << gt[ 0 ] << ", " 
	//							  << gt[ 1 ] << ", "
	//							  << gt[ 2 ] << ", "
	//							  << gt[ 3 ] << " ] ";
	//    std::cout << "\n\tEST -> [ " << est[ 0 ] << ", " 
	//							   << est[ 1 ] << ", "
	//							   << est[ 2 ] << ", "
	//							   << est[ 3 ] << " ] ";

	//    std::cout << "\n\tError -> [ " << err[ 0 ] << ", " 
	//							     << err[ 1 ] << ", "
	//							     << err[ 2 ] << ", "
	//							     << err[ 3 ] << " ] ";
		errLen = err.norm();
		errSum += errLen;
	//	std::cout << "\n\tErrLen: " << errLen << std::endl;
	}
	std::cout << "Summed Point Error: " << errSum << std::endl;
	
	// go over all keyframes and compare the result
	Eigen::Matrix4d errMat;
	errSum = 0;
	for( size_t i = 0; i < groundTruth.numKeyframes(); i++ ){
		const Eigen::Matrix4d & gt = groundTruth.keyframeForId( i ).pose().transformation();
		const Eigen::Matrix4d & est = optimized.keyframeForId( i ).pose().transformation();
		errMat = gt - est;
		errSum += errMat.array().square().sum();		
	}
	std::cout << "Summed Cam Error: " << errSum << std::endl;

}

void saveMapToXml( const SlamMap& map, const String& fileName )
{
	XMLDocument doc;
	doc.addNode( map.serialize() );
	doc.save( fileName );
}

int main(int argc, char* argv[])
{
	if( argc < 2 ){
		std::cout << "Usage: " << argv[ 0 ] << " <7|9|54>" << std::endl;
		return 0;
	}

	int n = atoi( argv[ 1 ] );

	cvt::Resources resources;
	SlamMap map, gtMap;
		
	try {
		String intrinsicsFile = resources.find("sba/calib.txt");
		String camFile, pointFile, camGT, pointGT;	
		switch( n ){
			case 7:	
				camFile = resources.find("sba/7cams.txt");
				pointFile = resources.find("sba/7pts.txt");
				camGT = resources.find("sba/resultCams7.txt");
				pointGT = resources.find("sba/resultPts7.txt");
				break;
			case 9:
				camFile = resources.find("sba/9cams.txt");
				pointFile = resources.find("sba/9pts.txt");
				camGT = resources.find("sba/resultCams9.txt");
				pointGT = resources.find("sba/resultPts9.txt");
				break;
			case 54:
				camFile = resources.find("sba/54cams.txt");
				pointFile = resources.find("sba/54pts.txt");
				camGT = resources.find("sba/resultCams54.txt");
				pointGT = resources.find("sba/resultPts54.txt");
				break;
			default:
				throw CVTException( "Wrong file specified: 7, 9 or 54" );
				break;
		}

		parseLourakisData( map, gtMap, intrinsicsFile, camFile, pointFile, camGT, pointGT );

		saveMapToXml( map, "map.xml" );
		return 0;

		SparseBundleAdjustment sba;
		TerminationCriteria<double> termcrit;
		termcrit.setCostThreshold( 0.1 );
		termcrit.setMaxIterations( 100 );


		Time timer;
		sba.optimize( map, termcrit );
		std::cout << "SBA took: " << timer.elapsedMilliSeconds() << "ms" << std::endl;	
		std::cout << "Iterations: " << sba.iterations() <<", Final epsilon: " << sba.costs() << std::endl; 
		std::cout << "Time/iteration: " << timer.elapsedMilliSeconds() / sba.iterations() << "ms" << std::endl;
		
		compareMaps( gtMap, map );

	}
	catch (const cvt::Exception & e) {
		std::cout << e.what() << std::endl;
	}	
	
	return 0;
}
