#ifndef CVT_MODEL_H
#define CVT_MODEL_H

#include <cvt/math/Vector.h>
#include <vector>

namespace cvt {
	class Model {
		public:
			Model();
			~Model();

			size_t vertexSize();
			size_t normalsSize();
			size_t texcoordSize();
			size_t trianglesSize();

			void addVertex( const Vector3f& vtx );
			void addNormal( const Vector3f& normal );
			void addTexCoord( const Vector2f& texcoord );

			void addTriangle( size_t idxv1, size_t idxv2, size_t idxv3 );
			void addTriangle( size_t idxv1, size_t idxv2, size_t idxv3,
							  size_t idxn1, size_t idxn2, size_t idxn3 );
			void addTriangle( size_t idxv1, size_t idxv2, size_t idxv3,
							  size_t idxn1, size_t idxn2, size_t idxn3,
							  size_t idxt1, size_t idxt2, size_t idxt3 );

			const Vector3f& vertex( size_t i ) const;
			const Vector3f& normal( size_t i ) const;
			const Vector2f& texcoord( size_t i ) const;

			void clear();

		private:
			std::vector<Vector3f>	_vertices;
			std::vector<Vector3f>	_normals;
			std::vector<Vector2f>	_texcoords;
			std::vector<size_t>	_triVertices;
			std::vector<size_t>	_triNormals;
			std::vector<size_t>	_triTexcoords;
	};

	inline size_t Model::vertexSize()
	{
		return _vertices.size();
	}

	inline size_t Model::normalsSize()
	{
		return _normals.size();
	}

	inline size_t Model::texcoordSize()
	{
		return _texcoords.size();
	}

	inline size_t Model::trianglesSize()
	{
		return _triVertices.size();
	}

	inline const Vector3f& Model::vertex( size_t i ) const
	{
		return _vertices[ i ];
	}

	inline const Vector3f& Model::normal( size_t i ) const
	{
		return _normals[ i ];
	}

	inline const Vector2f& Model::texcoord( size_t i ) const
	{
		return _texcoords[ i ];
	}

	inline void Model::addVertex( const Vector3f& vtx )
	{
		_vertices.push_back( vtx );
	}

	inline void Model::addNormal( const Vector3f& normal )
	{
		_normals.push_back( normal );
	}

	inline void Model::addTexCoord( const Vector2f& texcoord )
	{
		_texcoords.push_back( texcoord );
	}

	inline void Model::addTriangle( size_t idxv1, size_t idxv2, size_t idxv3 )
	{
		_triVertices.push_back( idxv1 );
		_triVertices.push_back( idxv2 );
		_triVertices.push_back( idxv3 );
	}

	inline void Model::clear()
	{
		_vertices.clear();
		_normals.clear();
		_texcoords.clear();
		_triVertices.clear();
		_triNormals.clear();
		_triTexcoords.clear();
	}
}

#endif
