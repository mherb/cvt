/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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

			void draw();

		private:
			GLVertexArray _vao;
			GLBuffer _vertices;
			GLBuffer _normals;
			GLBuffer _texcoords;
			GLBuffer _indices;
			size_t	 _numIndices;
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
			_numIndices = faces.size();
			_indices.alloc( GL_STATIC_DRAW, sizeof( GLuint ) * _numIndices, &faces[ 0 ] );

		} else {
			_numIndices = mesh.faceSize() * 3;
			_indices.alloc( GL_STATIC_DRAW, sizeof( GLuint ) * _numIndices, mesh.faces() );
		}

		_vao.setVertexData( _vertices, 3, GL_FLOAT );
		_vao.setTexCoordData( _texcoords, 2, GL_FLOAT );
		_vao.setNormalData( _normals, 3, GL_FLOAT );

		_vao.setColor( Color::GRAY );
	}

	inline GLMesh::~GLMesh()
	{
	}



	inline void GLMesh::draw()
	{
		_vao.drawIndirect( _indices, GL_UNSIGNED_INT, GL_TRIANGLES, _numIndices ); //FIXME: GL3 has no support for GL_QUADS ...
	}
}

#endif
