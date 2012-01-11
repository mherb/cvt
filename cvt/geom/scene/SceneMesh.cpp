#include <cvt/geom/scene/SceneMesh.h>

namespace cvt {

	void SceneMesh::removeRedundancy( float vepsilon, float nepsilon, float tepsilon )
	{
		std::vector<Vector3f>		nvertices;
		std::vector<Vector3f>		nnormals;
		std::vector<Vector2f>		ntexcoords;
		std::vector<unsigned int>	nvindices;

		for( size_t idx = 0; idx < _vertices.size(); idx++ ) {
			Vector3f v = _vertices[ idx ];
			bool added = false;

			for( size_t i = 0; i < nvertices.size() && !added; i++ ) {
				if( v.isEqual( nvertices[ i ], vepsilon ) ) {
					if( normalSize() ) {
						Vector3f n = _normals[ idx ];
						if( !n.isEqual( nnormals[ i ], nepsilon ) )
							continue;
					}
					if( texcoordSize() ) {
						Vector2f t = _texcoords[ idx ];
						if( !t.isEqual( ntexcoords[ i ], tepsilon ) )
							continue;
					}
					added = true;
					nvindices.push_back( idx );
				}
			}
			if( !added ) {
				nvertices.push_back( v );
				if( normalSize() )
					nnormals.push_back( _normals[ idx ] );
				if( texcoordSize() )
					ntexcoords.push_back( _texcoords[ idx ] );
				nvindices.push_back( nvertices.size() - 1 );
			}
		}

		_vertices = nvertices;
		_normals = nnormals;
		_texcoords = ntexcoords;
		_vindices = nvindices;
	}

	void SceneMesh::quadsToTriangles()
	{
		const unsigned int table[] = { 0, 1, 2, 2, 3, 0 };

		if( _type != SCENEMESH_QUADS || _vindices.size() % 4 != 0 )
			return;

		std::vector<unsigned int> nvindices;
		size_t size = _vindices.size();
		for( size_t n = 0; n < size; n += 4 ) {
				// Convert quad to triangle
				for( int i = 0; i < 6; i++ ) {
					nvindices.push_back( _vindices[ n + table[ i ] ] );
				}
		}

		_vindices = nvindices;
		_type = SCENEMESH_TRIANGLES;
	}

}
