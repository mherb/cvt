#include <cvt/geom/Model.h>

#include <string.h>

namespace cvt {
	Model::Model()
	{
	}

	Model::~Model()
	{
	}


	void Model::calcNormals()
	{
		Vector3f v1, v2, n;
		size_t idx1, idx2, idx3;

		/* clear all normals */
		_normals.clear();
		_normals.resize( _vertices.size() );

		/* normal indices are the same as the vertex indices */
		_triNormals = _triVertices;

		/* store the number of normals per vertex */
		memset( &_normals[ 0 ], 0, sizeof( Vector3f ) * _vertices.size() );

		/* for each face calc the normal and add the result to the normals of the corresponding vertices */
		size_t nvertices = _triVertices.size() / 3;
		for( size_t i = 0; i < nvertices; i++ )
		{
			idx1 = _triVertices[ i * 3 + 0 ];
			idx2 = _triVertices[ i * 3 + 1 ];
			idx3 = _triVertices[ i * 3 + 2 ];
			v1   = _vertices[ idx2 ];
			v2   = _vertices[ idx3 ];
			n    = _vertices[ idx1 ];
			v1  -= n;
			v2  -= n;

			v1.normalize();
			v2.normalize();
			n = v1.cross( v2 );

			_normals[ idx1 ] += n;
			_normals[ idx2 ] += n;
			_normals[ idx3 ] += n;
		}

		for( size_t i = 0; i < _normals.size(); i++ )
		{
			if( _normals[ i ].normalize() <= Math::EPSILONF )
				_normals[ i ].set( 1.0f, 0.0f, 0.0f );
		}
	}
}
