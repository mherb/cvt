#include <iostream>
#include <fstream>
#include <vector>
#include <cvt/geom/PointSet.h>
#include <cvt/io/FileSystem.h>
#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/PCA.h>

using namespace cvt;

void readPoints( PointSet2f& pts, const std::string& path )
{
	std::ifstream file;
	file.open( path.c_str() );
	while( !file.eof() ) {
		Vector2f pt;
		std::string rest;
		int dummy;
		file >> dummy;
		file >> dummy;
		file >> pt.x >> pt.y;
		std::getline( file, rest );
		pts.add( pt );
	}
	file.close();
}

int main( int argc, char** argv )
{
	if( argc != 2 ) {
		std::cerr << "usage: " << argv[ 0 ] << " asf-files-path" << std::endl;
		return 1;
	}

	std::vector<std::string> files;
	FileSystem::filesWithExtension( argv[ 1 ], files, "asf" );

	std::vector<PointSet2f> allpts;
	allpts.resize( files.size() );

	for( size_t i = 0; i < files.size(); i++ ) {
		readPoints( allpts[ i ], files[ i ] );
	}

	for( size_t i = 1; i < files.size(); i++ ) {
		Matrix3f sim;
		allpts[ 0 ].alignSimilarity( allpts[ i ] );
		allpts[ i ].transform( sim );
	}

	PCAf pca( 58 );
	for( size_t i = 0; i < files.size(); i++ ) {
		pca.addSample( ( float* ) &allpts[ 0 ][ 0 ] );
	}

	Eigen::Matrix<float, Eigen::Dynamic, 1> mean( 58 );
	Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> pc( 58, 58 );
	pca.mean( mean );
	pca.principleComponents( pc );

	std::cout << "Mean :\n" << mean << std::endl;
	std::cout << "PCs :\n" << pc << std::endl;
}
