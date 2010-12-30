#ifndef CVT_MODEL_H
#define CVT_MODEL_H

#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/geom/Box.h>
#include <vector>

namespace cvt {
	class Model {
		public:
			Model();
			~Model();

			size_t vertexSize() const;
			size_t normalsSize() const;
			size_t texcoordSize() const;
			size_t trianglesSize() const;

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
			size_t index( size_t i ) const;

			void	 cleanup();
			void	 calcNormals();
			void	 calcAABB( Boxf& box ) const;
			Vector3f meanVertex(  ) const;
			void	 translate( const Vector3f& translation );
			void     scale( float scale );
			void	 transform( const Matrix3f& mat );
			void	 transform( const Matrix4f& mat );
			void	 center();
			void	 clear();

		private:
			std::vector<Vector3f>	_vertices;
			std::vector<Vector3f>	_normals;
			std::vector<Vector2f>	_texcoords;
			std::vector<size_t>	_triVertices;
			std::vector<size_t>	_triNormals;
			std::vector<size_t>	_triTexcoords;
	};

	inline size_t Model::vertexSize() const
	{
		return _vertices.size();
	}

	inline size_t Model::normalsSize() const
	{
		return _normals.size();
	}

	inline size_t Model::texcoordSize() const
	{
		return _texcoords.size();
	}

	inline size_t Model::trianglesSize() const
	{
		return _triVertices.size() / 3;
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

	inline size_t Model::index( size_t i ) const
	{
		return _triVertices[ i ];
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


	inline Vector3f Model::meanVertex( ) const
	{
		float inv;
		size_t n = _vertices.size();
		const Vector3f* pt = &_vertices[ 0 ];
		Vector3f mean;

		mean.zero();
		while( n-- )
			mean += *pt++;
		inv = 1.0f / ( float ) _vertices.size();
		mean *= inv;
		return mean;
	}


	inline void Model::translate( const Vector3f& translation )
	{
		size_t n = _vertices.size();
		Vector3f* pt = &_vertices[ 0 ];

		while( n-- )
			*pt++ += translation;
	}

	inline void Model::scale( float scale )
	{
		size_t n = _vertices.size();
		Vector3f* pt = &_vertices[ 0 ];

		while( n-- )
			*pt++ *= scale;
	}

	inline void Model::transform( const Matrix3f& mat )
	{
		size_t n = _vertices.size();
		Vector3f* pt = &_vertices[ 0 ];

		while( n-- ) {
			*pt = mat * *pt;
			pt++;
		}
	}

	inline void Model::transform( const Matrix4f& mat )
	{
		size_t n = _vertices.size();
		Vector3f* pt = &_vertices[ 0 ];

		/* if last row is [ 0 0 0 1 ], split mat into 3 x 3 matrix and translation */
		if( mat[ 3 ] == Vector4f( 0, 0, 0, 1.0f ) ) {
			Matrix3f _mat( mat );
			Vector3f trans( mat[ 0 ][ 3 ], mat[ 1 ][ 3 ], mat[ 2 ][ 3 ] );
			while( n-- ) {
				*pt = _mat * *pt;
				*pt += trans;
				pt++;
			}
		} else {
			while( n-- ) {
				*pt = mat * *pt;
				pt++;
			}
		}
	}

	inline void Model::cleanup()
	{
		if( normalsSize() != vertexSize() )
			calcNormals();
	}

	inline void Model::center()
	{
		Vector3f m = meanVertex();
		translate( -m );
	}

	inline void Model::calcAABB( Boxf& box ) const
	{
		Vector3f min, max;
		min = max = _vertices[ 0 ];
		for( size_t i = 1; i < _vertices.size(); i++ ) {
			min.x = Math::min( min.x, _vertices[ i ].x );
			min.y = Math::min( min.y, _vertices[ i ].y );
			min.z = Math::min( min.z, _vertices[ i ].z );
			max.x = Math::max( max.x, _vertices[ i ].x );
			max.y = Math::max( max.y, _vertices[ i ].y );
			max.z = Math::max( max.z, _vertices[ i ].z );
		}
		box.set( min, max );
	}

	inline std::ostream& operator<<( std::ostream& out, const Model& mdl )
	{
		out << "Model:" << std::endl;
		out << "\t  vertices: " << mdl.vertexSize() << std::endl;
		out << "\t   normals: " << mdl.normalsSize() << std::endl;
		out << "\ttex-coords: " << mdl.texcoordSize() << std::endl;
		out << "\t triangles: " << mdl.trianglesSize();
		return out;
	}

}

#endif
