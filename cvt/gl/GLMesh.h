#ifndef CVT_GLMESH_H
#define CVT_GLMESH_H

#include <cvt/gl/GLVertexArray.h>
#include <cvt/geom/scene/SceneMesh.h>

namespace cvt {
	class GLMesh {
		public:
			GLMesh();
			GLMesh( const SceneMesh& mesh );
			/*GLMesh( const Vector3f* vertices, const Vector3f* normals, const Vector2f* texcoords, size_t vsize,
				    const unsigned int* indices, size_t isize, MeshType type );*/
			~GLMesh();

			GLBuffer& vertices();
			GLBuffer& normals();
			GLBuffer& texcoords();
			GLBuffer& faceIndices();

			size_t&   numTriangles();
			size_t	  numTriangles() const;

			void draw();

		private:
			GLVertexArray _vao;
			GLBuffer _vertices;
			GLBuffer _normals;
			GLBuffer _texcoords;
			GLBuffer _indices;
			size_t	 _numTris;
	};

	inline GLMesh::GLMesh() :
		_vertices( GL_ARRAY_BUFFER ),
		_normals( GL_ARRAY_BUFFER ),
		_texcoords( GL_ARRAY_BUFFER ),
		_indices( GL_ELEMENT_ARRAY_BUFFER )

	{
	}

	inline GLMesh::GLMesh( const SceneMesh& mesh ) :
		_vertices( GL_ARRAY_BUFFER ),
		_normals( GL_ARRAY_BUFFER ),
		_texcoords( GL_ARRAY_BUFFER ),
		_indices( GL_ELEMENT_ARRAY_BUFFER )
	{
		_vertices.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 3 * mesh.vertexSize(), mesh.vertices() );
		_normals.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 3 * mesh.normalSize(), mesh.normals() );
		_texcoords.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 2 * mesh.texcoordSize(), mesh.texcoords() );
		if( mesh.meshType() == SCENEMESH_QUADS ) {
			std::vector<unsigned int> faces;
			mesh.facesTriangles( faces );
			_numTris = faces.size() / 3;
			_indices.alloc( GL_STATIC_DRAW, sizeof( GLuint ) * _numTris * 3, &faces[ 0 ] );

		} else {
			_numTris = mesh.faceSize();
			_indices.alloc( GL_STATIC_DRAW, sizeof( GLuint ) * _numTris * 3, mesh.faces() );
		}

		_vao.setVertexData( _vertices, 3, GL_FLOAT );
		_vao.setTexCoordData( _texcoords, 2, GL_FLOAT );
		_vao.setNormalData( _normals, 3, GL_FLOAT );

		_vao.setColor( Color::GRAY );
	}

	inline GLMesh::~GLMesh()
	{
	}

	inline GLBuffer& GLMesh::vertices()
	{
		return _vertices;
	}

	inline GLBuffer& GLMesh::normals()
	{
		return _normals;
	}

	inline GLBuffer& GLMesh::texcoords()
	{
		return _texcoords;
	}

	inline GLBuffer& GLMesh::faceIndices()
	{
		return _indices;
	}

	inline size_t& GLMesh::numTriangles()
	{
		return _numTris;
	}

	inline size_t GLMesh::numTriangles() const
	{
		return _numTris;
	}


	inline void GLMesh::draw()
	{
		if( _indices.size() )
			_vao.drawIndirect( _indices, GL_UNSIGNED_INT, GL_TRIANGLES, _numTris * 3 );
		else
			_vao.draw( GL_TRIANGLES, 0, _numTris );

	}
}

#endif
