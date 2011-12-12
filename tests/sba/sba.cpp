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
#include <cvt/math/SparseBundleAdjustment.h>
#include <cvt/math/SBAData.h>

#include <cvt/vision/SlamMap.h>
#include <cvt/vision/CameraCalibration.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace cvt;

// test app for sparse bundle adjustment, 
// test input files from the sba-lib:
struct Point3d 
{	
	Point3d( double x, double y, double z ):
		P( x, y, z )
	{		
	}
			
	Eigen::Vector3d P;
	std::vector<Eigen::Vector2d> observations;
	std::vector<size_t> frameIds;
};

struct CameraTransform {
	Eigen::Quaterniond orientation;
	Eigen::Vector3d	 translation;
};

void parseCalibFile( const std::string & file, Eigen::Matrix3d & K )
{
	std::ifstream fileIn( file.c_str() );
	std::istringstream lineBuffer;

	double val;
	size_t row = 0;
	size_t col = 0;

	std::string line;
	while ( std::getline( fileIn, line ) ){
		lineBuffer.clear();
		lineBuffer.str( line );
		col = 0;
		while ( lineBuffer >> val ){
			K( row, col++ ) = val;
		}
		row++;
	}	
	
	fileIn.close();	
}

void parseCameraFile( const std::string & file, std::vector<CameraTransform> & cameras )
{
	std::ifstream fileIn( file.c_str() );
	std::istringstream lineBuffer;
	
	double val;
	
	CameraTransform cam;
	
	std::string line;
	while ( std::getline( fileIn, line ) ){
		lineBuffer.clear();
		lineBuffer.str( line );
		
		lineBuffer >> val; cam.orientation.w() = val;
		lineBuffer >> val; cam.orientation.x() = val;
		lineBuffer >> val; cam.orientation.y() = val;
		lineBuffer >> val; cam.orientation.z() = val;
		lineBuffer >> val; cam.translation.x() = val;
		lineBuffer >> val; cam.translation.y() = val;
		lineBuffer >> val; cam.translation.z() = val;
		
		cameras.push_back( cam );
	}	
	
	fileIn.close();	
}

void parsePtsFile( const std::string & file, std::vector<Point3d> & points3d )
{
	std::ifstream fileIn( file.c_str() );
	std::istringstream lineBuffer;
	
	double x, y, z;
	size_t frameId;
	
	unsigned int numFrames;
	unsigned int currentLine = 0;
	
	std::string line;
	
	// step over first line
	std::getline( fileIn, line );
	while ( std::getline( fileIn, line ) ){
		currentLine++;

		lineBuffer.clear();
		lineBuffer.str( line );
		
		lineBuffer >> x; lineBuffer >> y; lineBuffer >> z;
		points3d.push_back( Point3d( x, y, z ) );
		
		lineBuffer >> numFrames;		
		while (numFrames--) {
			lineBuffer >> frameId;
			lineBuffer >> x; lineBuffer >> y;
			
			points3d.back().observations.push_back( Eigen::Vector2d( x, y ) );
			points3d.back().frameIds.push_back( frameId );
		}
	}	
	
	fileIn.close();	
}

void loadGroundTruth( const std::string & cams, 
					  const std::string & pts, 
					  std::vector<Eigen::Matrix3d> & rotations, 
					  std::vector<Eigen::Vector3d> & translations,
					  std::vector<Eigen::Vector3d> & points )
{
	std::ifstream camFile( cams.c_str() );
	std::istringstream lineBuffer;
		
	Eigen::Quaterniond q;
		
	std::string line;

	// skip first line
	std::getline( camFile, line );

	while ( std::getline( camFile, line ) ){
		lineBuffer.clear();
		lineBuffer.str( line );
		
		lineBuffer >> q.w();
		lineBuffer >> q.x();
		lineBuffer >> q.y();
		lineBuffer >> q.z();
		rotations.push_back( q.toRotationMatrix() );
		
		translations.push_back( Eigen::Vector3d::Zero() );
		lineBuffer >> translations.back()(0);
		lineBuffer >> translations.back()(1);
		lineBuffer >> translations.back()(2);
	}	
	camFile.close();	
	
	std::ifstream ptsFile( pts.c_str() );
	std::getline( ptsFile, line );
	
	while ( std::getline( ptsFile, line ) ){
		lineBuffer.clear();
		lineBuffer.str( line );
		
		points.push_back( Eigen::Vector3d::Zero() );
				
		lineBuffer >> points.back()(0);
		lineBuffer >> points.back()(1);
		lineBuffer >> points.back()(2);
	}	
	ptsFile.close();	
}

void printLoadedData( Eigen::Matrix3d & K, 
					  std::vector<CameraTransform> & cameras, 
					  std::vector<Point3d> & points3d )
{
	std::cout << "Intrinsics:\n" << K << std::endl;
	
	for(unsigned int i = 0; i < cameras.size(); i++){
		std::cout << "Camera " << i << std::endl;
		std::cout << "\tOrientation: " 
		<< cameras[ i ].orientation.w() << ", " 
		<< cameras[ i ].orientation.x() << ", "
		<< cameras[ i ].orientation.y() << ", "
		<< cameras[ i ].orientation.z() << std::endl;
		
		std::cout << "\tTranslation" << cameras[ i ].translation( 0 ) << ", " 
									 << cameras[ i ].translation( 1 ) << ", " 
									 << cameras[ i ].translation( 2 ) << std::endl;
	}
	
	std::cout << "Observations for 3D Points:" << std::endl;
	for(unsigned int i = 0; i < points3d.size(); i++){
		std::cout << "3D Point:" << points3d[ i ].P.x() << ", " << points3d[ i ].P.y() << ", " << points3d[ i ].P.z() << std::endl;
		
		for(unsigned int k = 0; k < points3d[ i ].observations.size(); k++){
			std::cout << "\tId: " << points3d[ i ].frameIds[ k ];
			std::cout << "\tp: " << points3d[ i ].observations[ k ].x() << ", " << points3d[ i ].observations[ k ].y() << std::endl;
		}
	}
}

void convertData(Eigen::Matrix3d & K, 
				 std::vector<CameraTransform> & cameras, 
				 std::vector<Point3d> & points3d,
				 cvt::SBAData & sbaData)
{
	std::vector<cvt::KeyFrame*> keyFrames( cameras.size() );
	
	for( unsigned int i = 0; i < cameras.size(); i++ ){
		keyFrames[ i ] = new cvt::KeyFrame( K, i );
		cvt::CameraModel & cam =  keyFrames[ i ]->camera;
		cam.set( cameras[ i ].orientation, cameras[ i ].translation );
		cam.updateProjectionMatrix();
		
		sbaData.addKeyFrame( keyFrames[ i ] );
	}
	
	for( unsigned int i = 0; i < points3d.size(); i++ ){
		Eigen::Vector4d p;
		p.block(0, 0, 3, 1) = points3d[ i ].P; 
		p(3)=1.0;
		size_t id;
		
		Eigen::Vector4d * point = sbaData.addPoint( p, id );
		
		size_t view;
		
		for( unsigned int v = 0; v < points3d[ i ].frameIds.size(); ++v ){
			view = points3d[ i ].frameIds[ v ];
			
			keyFrames[ view ]->addMeasurement( point, id, points3d[ i ].observations[ v ] );
			
			sbaData.addFrameIdForPoint(id, view );	
		}					
	}	
	sbaData.updateCosts();
	
//	std::cout << *(sbaData.frames()[0]->measurements[0].point3d->data) << std::endl;

}

void showResults( cvt::SBAData & sbaData )
{	
	// Cameras:
	for( unsigned int i = 0; i < sbaData.frames().size(); i++ ){
		std::cout << "Camera " << i << std::endl;		
		Eigen::Quaterniond q( sbaData.frames()[ i ]->camera.R() );
		const Eigen::Vector3d & t = sbaData.frames()[ i ]->camera.t();
		std::cout << "\tRotation: [ " << q.w() << ", " << q.x() << ", " << q.y() << ", " << q.z() << "]" << std::endl;
		std::cout << "\tTranslation: [ " << t.x() << ", " << t.y() << ", " << t.z() << "]" << std::endl;
		std::cout << std::endl;
	}
	
	sbaData.printPoints();
}

void compareResults( cvt::SBAData & sbaData, 
					 std::vector<Eigen::Matrix3d> & rot,
					 std::vector<Eigen::Vector3d> & trans,
					 std::vector<Eigen::Vector3d> & pts )
{
	// TODO: 
	// compare the results against the result 
	// from the sba package or GT data in simulation case
	
	Eigen::Vector3d tmpVec;
	Eigen::Matrix3d tmpMat;
	
	double RDiff = 0.0;
	double TDiff = 0.0;
	double n;
	for( unsigned int i = 0; i < rot.size(); i++ ){
		cvt::CameraModel & cam = sbaData.cameraWithId( i );
		
		// compare rotation matrices
		tmpMat = ( cam.R() - rot[ i ] );
		
		/*
		std::cout << "Cam " << i << " GT -> EST " << std::endl;
		std::cout << cam.R()(0, 0) << " " << cam.R()(0, 1) << " " << cam.R()(0, 2) << "\t\t" << rot[i](0, 0) << " " << rot[i](0, 1) << " " << rot[i](0, 2) << std::endl;
		std::cout << cam.R()(1, 0) << " " << cam.R()(1, 1) << " " << cam.R()(1, 2) << "\t\t" << rot[i](1, 0) << " " << rot[i](1, 1) << " " << rot[i](1, 2) << std::endl;
		std::cout << cam.R()(2, 0) << " " << cam.R()(2, 1) << " " << cam.R()(2, 2) << "\t\t" << rot[i](2, 0) << " " << rot[i](2, 1) << " " << rot[i](2, 2) << std::endl;
		std::cout << std::endl;
		 */
		
		for( unsigned int r = 0; r < 3; r++ ){
			for( unsigned int c = 0; c < 3; c++ ){
				RDiff += (tmpMat( r, c ) * tmpMat( r, c ));
			}			
		}		
		
		// compare translations
		tmpVec = ( cam.t() - trans[ i ] );
		n = tmpVec.norm();		
		
		TDiff += n;
	}
	
	double pointDiff = 0.0;	
	for( unsigned int i = 0; i < pts.size(); i++ ){
		// calc point error (difference between the two points)
		const Eigen::Vector4d & p4 = sbaData.pointWithId( i );
		tmpVec(0) =  p4( 0 ) / p4( 3 ) - pts[i]( 0 ); 
		tmpVec(1) =  p4( 1 ) / p4( 3 ) - pts[i]( 1 );
		tmpVec(2) =  p4( 2 ) / p4( 3 ) - pts[i]( 2 );
		pointDiff += tmpVec.norm();
		
		/*
		std::cout << "Point ( GT -> EST ) " << i << std::endl;
		std::cout << "\t" << pts[ i ]( 0 ) <<" -> " << p4( 0 ) << std::endl; 
		std::cout << "\t" << pts[ i ]( 1 ) <<" -> " << p4( 1 ) << std::endl; 
		std::cout << "\t" << pts[ i ]( 2 ) <<" -> " << p4( 2 ) << std::endl; 
		std::cout << std::endl;
		 */
	}
	std::cout << "Average Cam SSD: R -> " << RDiff / rot.size() << " t -> " << TDiff / trans.size() << std::endl;
	std::cout << "Average P SSD: " << pointDiff / pts.size() << std::endl;
	
}

void testMVGImplementation( Eigen::Matrix3d & K, 
						    std::vector<CameraTransform> & cameras, 
						    std::vector<Point3d> & points3d,
						    std::vector<Eigen::Matrix3d> & rot,
						    std::vector<Eigen::Vector3d> & trans,
						    std::vector<Eigen::Vector3d> & pts )
{
	cvt::SBAData sbaData;
	cvt::SparseBundleAdjustment sba;
	sba.setTerminationCriteria( 1e-12, 21 );
	
	convertData( K, cameras, points3d, sbaData );
	
	cvt::Time timer;
	sba.optimize( sbaData );
	
	std::cout << "MVG BA took: " << timer.elapsedMilliSeconds() << "ms" << std::endl;	
	std::cout << "Iterations: " << sba.iterations() <<", Final epsilon: " << sba.epsilon() << std::endl; 
	std::cout << "Time/iteration: " << timer.elapsedSeconds() / sba.iterations() << std::endl;
	
	//std::cout << "Results" << std::endl;
	//showResults( sbaData );	
	compareResults( sbaData, rot, trans, pts );
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
	Eigen::Vector2d meas;

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
			meas.x() = lineTokens[ tokIter++ ].toDouble();
			meas.y() = lineTokens[ tokIter++ ].toDouble();
			map.addMeasurement( pointId, keyframeId, meas );
		}

		lineTokens.clear();
	}
	std::cout << numPts << std::endl;
}

void parseLourakisData( SlamMap & slamMap, 
						const String & calibFile, 
						const String & camFile, 
						const String & ptsFile )
{
	CameraCalibration camCalib;
	parseIntrinsics( camCalib, calibFile );
	parseCamFile( slamMap, camFile );
	parsePointFile( slamMap, ptsFile );
}

int main(int argc, char* argv[])
{
	cvt::Resources resources;

	// single intrinsics:	
	Eigen::Matrix3d K;

	std::vector<CameraTransform> cameras;
	std::vector<Point3d> points3d;
	SlamMap map;
		
	try {
		
		String intrinsicsFile = resources.find("sba/calib.txt");
		
		String camFile = resources.find("sba/7cams.txt");
		String pointFile = resources.find("sba/7pts.txt");
		String camGT = resources.find("sba/resultCams7.txt");
		String pointGT = resources.find("sba/resultPts7.txt");
		
		/*
		String camFile = resources.find("sba/9cams.txt");
		String pointFile = resources.find("sba/9pts.txt");
		String camGT = resources.find("sba/resultCams9.txt");
		String pointGT = resources.find("sba/resultPts9.txt");
		*/
	/*	
		String camFile = resources.find("sba/54cams.txt");
		String pointFile = resources.find("sba/54pts.txt");
		String camGT = resources.find("sba/resultCams54.txt");
		String pointGT = resources.find("sba/resultPts54.txt");		
	*/

		parseLourakisData( map, intrinsicsFile, camFile, pointFile );

/*
		std::vector<Eigen::Matrix3d> gtRots;
		std::vector<Eigen::Vector3d> gtTrans;
		std::vector<Eigen::Vector3d> gtPts;		
		
		parseCalibFile( intrinsicsFile, K );		
		parseCameraFile( camFile, cameras );
		parsePtsFile( pointFile, points3d );		
		loadGroundTruth( camGT, pointGT, gtRots, gtTrans, gtPts );		
		testMVGImplementation( K, cameras, points3d, gtRots, gtTrans, gtPts );
		*/
		
		//testSimulation();
	}
	catch (const cvt::Exception & e) {
		std::cout << e.what() << std::endl;
	}	
	
	return 0;
}
