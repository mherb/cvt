#include <cvt/geom/MarchingCubes.h>
#include <cvt/geom/scene/SceneMesh.h>

#include <stdio.h>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 3 ) {
		std::cout << "usage: " << argv[ 0 ] << " grid-size grid-file" << std::endl;
		return 0;
	}

	int grid = String( argv[ 1 ]).toInteger();
	std::cout << "Grid: " << grid << " x " << grid << " x " << grid << std::endl;
	FILE* f = fopen( argv[ 2 ], "rb" );
	float* volume = new float[ grid * grid * grid ];
	fread( volume, sizeof( float ), grid * grid * grid, f );
	fclose( f );

	{
		MarchingCubes mc( volume, grid, grid, grid );
		SceneMesh mesh( "MyMesh" );
		mc.triangulateWithNormals( mesh );
//		mesh.removeRedundancy( 1e-2f );
//		mesh.calculateNormals( 5.0f );

		f = fopen( "mcresult.obj", "wb" );
		for( size_t idx = 0; idx < mesh.vertexSize(); idx++ ) {
			Vector3f vtx = mesh.vertex( idx );
			fprintf( f, "v %f %f %f\n", vtx.x, vtx.y, vtx.z );
		}

		for( size_t idx = 0; idx < mesh.normalSize(); idx++ ) {
			Vector3f vtx = mesh.normal( idx );
			fprintf( f, "vn %f %f %f\n", vtx.x, vtx.y, vtx.z );
		}

		const unsigned int* faces = mesh.faces();
		for( size_t idx = 0; idx < mesh.faceSize(); idx++ ) {
			fprintf( f, "f %d %d %d\n", *( faces) + 1, *( faces + 1 ) + 1, *( faces + 2 ) + 1);
			faces += 3;
		}

		fclose( f );
	}

	delete[] volume;
}
