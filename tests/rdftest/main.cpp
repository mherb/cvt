
#include <cvt/ml/rdf/RDFClassificationTrainer2D.h>

#include <cvt/math/Vector.h>

#include <cvt/util/String.h>
#include <cvt/util/Data.h>
#include <cvt/util/DataIterator.h>
#include <cvt/io/FileSystem.h>

using namespace cvt;

int main( int argc, char** argv )
{
	RDForestTrainerClassification2D rdftrainer( 2 );

    Data filedata;
    FileSystem::load( filedata, argv[ 1 ] );
	std::vector<Vector3f> traindata;

    DataIterator iter( filedata );
	iter.skip( "\t\n\r " );
	while( iter.hasNext() ) {
		Vector3f data;
		data.z = iter.nextDouble() - 1.0f;
		data.x = iter.nextDouble();
		data.y = iter.nextDouble();
		traindata.push_back( data );
		iter.skip( "\t\n\r " );
	};

	RDFNode<Vector2f,RDFClassHistogram<2> >* node =  rdftrainer.train( traindata, 3, 50000 );




	return 0;
}
