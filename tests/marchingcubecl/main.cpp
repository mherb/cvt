#include <cvt/gui/Application.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLBuffer.h>
#include <cvt/gl/GLBuffer.h>

#include "MarchingCubesCL.h"

#include <cvt/geom/MarchingCubes.h>
#include <cvt/geom/scene/SceneMesh.h>

#include <stdio.h>

using namespace cvt;

void writeOBJ( const float* vertices, const float* normals, size_t numtris )
{
		FILE* f;
		cl_float3* vert = ( cl_float3* ) vertices;
		cl_float3* norm = ( cl_float3* ) normals;

		f = fopen( "mcresult.obj", "wb" );
		size_t idxend = numtris * 3;
		for( size_t idx = 0; idx < idxend; idx++ ) {
			cl_float3 vtx = vert[ idx ];
			fprintf( f, "v %f %f %f\n", vtx.x, vtx.y, vtx.z );
			cl_float3 nrml = norm[ idx ];
			fprintf( f, "vn %f %f %f\n", nrml.x, nrml.y, nrml.z );
		}

		for( size_t idx = 0; idx < numtris; idx++ ) {
			fprintf( f, "f %lu//%lu %lu//%lu %lu//%lu\n",idx * 3 + 1, idx * 3 + 1, idx * 3 + 2, idx * 3 + 2, idx * 3 + 3, idx * 3 + 3);
		}

		fclose( f );
}


int main( int argc, char** argv )
{
	if( argc != 3 ) {
		std::cout << "usage: " << argv[ 0 ] << " grid-size grid-file" << std::endl;
		return 0;
	}

	Application::init();
	Application::defaultGLContext()->makeCurrent();
/*	std::vector<CLPlatform> platforms;
    CLPlatform::get( platforms );
    CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );*/

	int grid = String( argv[ 1 ]).toInteger();
	std::cout << "Grid: " << grid << " x " << grid << " x " << grid << std::endl;
	FILE* f = fopen( argv[ 2 ], "rb" );
	float* volume = new float[ grid * grid * grid * 2 ];
	fread( volume, sizeof( float ), grid * grid * grid * 2, f );
	fclose( f );

	std::cout << "Read volume" << std::endl;
	{
		SceneMesh mesh("TSDFMESH");
		MarchingCubes mc( volume, grid, grid, grid, true, 20.0f );
		mc.triangulateWithNormals( mesh, 0.0f );
		std::cout << "CPU-Triangles: " << mesh.faceSize() << std::endl;
	}


	try {
		std::cout << "Building kernels" << std::endl;
		CLBuffer buf( sizeof( float ) * 2 * grid * grid * grid );
		float* ptr = ( float* ) buf.map();
		memcpy( ptr, volume, sizeof( float ) * 2 * grid * grid * grid );
		buf.unmap( ptr );
		delete[] volume;

		MarchingCubesCL mccl;
		unsigned int size = mccl.triangleSize( buf, grid, grid, grid );
		std::cout << "Calculating number of triangles" << std::endl;
		std::cout << size << std::endl;

		GLBuffer glbufvtx, glbufnormals;
		glbufvtx.alloc( GL_DYNAMIC_COPY, sizeof( cl_float3 ) * size * 3 );
		glbufnormals.alloc( GL_DYNAMIC_COPY, sizeof( cl_float3 ) * size * 3 );

		CLBuffer clbufvtx( glbufvtx );
		CLBuffer clbufnormals( glbufnormals );
		clbufvtx.acquireGLObject();
		clbufnormals.acquireGLObject();
//		CLBuffer tribuf( sizeof( cl_float3 ) * size * 3 );
		mccl.extractTriangles( clbufvtx, clbufnormals, buf, grid, grid, grid );
		clbufvtx.releaseGLObject();
		clbufnormals.releaseGLObject();

		ptr = ( float* ) glbufvtx.map();
		float* ptr2 = ( float* ) glbufnormals.map();
		writeOBJ( ptr, ptr2, size );
		glbufvtx.unmap( );
		glbufnormals.unmap( );



	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

#if 0
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
#endif

}
