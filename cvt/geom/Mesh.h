#ifndef CVT_MESH_H
#define CVT_MESH_H

#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/geom/Box.h>

namespace cvt {
	enum MeshType {
		MESH_TRIANGLES,
		MESH_QUADS
	};

	class Mesh {
		public:
			Mesh( MeshType type = MESH_TRIANGLES );
			~Mesh();

			MeshType		type() const;

			size_t			vertexSize() const;
			size_t			normalSize() const;
			size_t			texcoordSize() const;
			size_t			faceSize() const;

			const Vector3f& vertex( size_t i ) const;
			const Vector3f& normal( size_t i ) const;
			const Vector2f& texcoord( size_t i ) const;

			Vector3f		centroid() const;
			Boxf			boundingBox() const;

			void			transform( const Matrix4f& mat );
			void			translate( const Vector3f& translation );
			void			scale( float scale );

			void			calculateNormals();
			void			calculateAdjacency();
			void			removeRedundancy( float vepsilon, float nepsilon, float tepsilon );
			void			simplify( float vepsilon );
			void			addNoise( float amount );
			void			flipNormals( );
			void			quadsToTriangles();
			void			subdivideCatmullClark();


		private:
			std::vector<Vector3f> _vertices;
			std::vector<Vector3f> _normals;
			std::vector<Vector2f> _texcoords;

			std::vector<size_t>   _vindices;
			std::vector<size_t>   _nindices;
			std::vector<size_t>   _tindices;

			std::vector<size_t>   _adjacency;

			MeshType			  _type;
	};

	inline Mesh::Mesh( MeshType type ) : _type( type )
	{
	}

	inline Mesh::~Mesh()
	{
	}

	inline MeshType Mesh::type() const
	{
		return _type;
	}

	inline size_t Mesh::vertexSize() const
	{
		return _vertices.size();
	}

	inline size_t Mesh::normalSize() const
	{
		return _normals.size();
	}

	inline size_t Mesh::texcoordSize() const
	{
		return _texcoords.size();
	}

	inline size_t Mesh::faceSize() const
	{
		size_t nface = _type == MESH_TRIANGLES ? 3 : 4;
		return _vindices.size() / nface;
	}

	inline const Vector3f& Mesh::vertex( size_t i ) const
	{
		return _vertices[ i ];
	}

	inline const Vector3f& Mesh::normal( size_t i ) const
	{
		return _normals[ i ];
	}

	inline const Vector2f& Mesh::texcoord( size_t i ) const
	{
		return _texcoords[ i ];
	}

	inline Vector3f Mesh::centroid( ) const
	{
		size_t n = _vertices.size();
		const Vector3f* pt = &_vertices[ 0 ];
		Vector3f centroid;

		centroid.setZero();
		while( n-- )
			centroid += *pt++;
		centroid /= ( float ) _vertices.size();
		return centroid;
	}


	inline Boxf Mesh::boundingBox();
	{
		Vector3f min, max;
		Boxf bbox

		if( !_vertices.size() )
			return bbox;

		min = max = _vertices[ 0 ];
		size_t n = _vertices.size() - 1;
		const Vector3f* pt = &_vertices[ 1 ];

		while( n-- ) {
			min.x = Math::min( min.x, pt->x );
			min.y = Math::min( min.y, pt->y );
			min.z = Math::min( min.z, pt->z );
			max.x = Math::max( max.x, pt->x );
			max.y = Math::max( max.y, pt->y );
			max.z = Math::max( max.z, pt->z );
			pt++;
		}
		bbox.set( min, max );
		return bbox;
	}

	inline void Mesh::translate( const Vector3f& translation )
	{
		size_t n = _vertices.size();
		Vector3f* pt = &_vertices[ 0 ];

		while( n-- )
			*pt++ += translation;
	}

	inline void Mesh::scale( float scale )
	{
		size_t n = _vertices.size();
		Vector3f* pt = &_vertices[ 0 ];

		while( n-- )
			*pt++ *= scale;
	}

	inline void Mesh::transform( const Matrix3f& mat )
	{
		size_t n = _vertices.size();
		Vector3f* pt = &_vertices[ 0 ];

		while( n-- ) {
			*pt = mat * *pt;
			pt++;
		}
	}

	inline void Mesh::transform( const Matrix4f& mat )
	{
		size_t n = _vertices.size();
		Vector3f* pt = &_vertices[ 0 ];

		if( mat[ 3 ] == Vector4f( 0, 0, 0, 1.0f ) ) {
			/* if last row is [ 0 0 0 1 ], split mat into 3 x 3 matrix and translation */
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

}

#endif
