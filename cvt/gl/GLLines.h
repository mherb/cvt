#ifndef CVT_GLLINES_H
#define CVT_GLLINES_H

#include <cvt/gl/GLVertexArray.h>
#include <cvt/geom/scene/SceneLines.h>
#include <cvt/gfx/Color.h>

namespace cvt {
	class GLLines {
		public:
			GLLines();
			GLLines( const SceneLines& pts );
			GLLines( const Boxf& box, const Color& c );
			/*GLLines( const Vector3f* vertices, const Vector3f* normals, const Vector2f* texcoords, size_t vsize,
				    const unsigned int* indices, size_t isize, MeshType type );*/
			~GLLines();

			void draw( GLenum mode = GL_LINES );

		private:
			GLVertexArray _vao;
			GLBuffer _vertices;
			GLBuffer _colors;
			size_t	 _numPts;
	};

	inline GLLines::GLLines() :
		_vertices( GL_ARRAY_BUFFER ),
		_colors( GL_ARRAY_BUFFER ),
		_numPts( 0 )
	{
	}


	inline GLLines::GLLines( const Boxf& box, const Color& c )
	{
		Vector3f pos, size;
		box.getPosition( pos );
		box.getSize( size );
		float vertices[ 24 * 3 ] = { /* top */
								 pos.x, pos.y, pos.z,
								 pos.x + size.x, pos.y, pos.z,
								 pos.x + size.x, pos.y, pos.z,
								 pos.x + size.x, pos.y, pos.z + size.z,
								 pos.x + size.x, pos.y, pos.z + size.z,
								 pos.x, pos.y, pos.z + size.z,
								 pos.x, pos.y, pos.z + size.z,
								 pos.x, pos.y, pos.z,
								 /* bottom */
								 pos.x, pos.y + size.y, pos.z,
								 pos.x + size.x, pos.y + size.y, pos.z,
								 pos.x + size.x, pos.y + size.y, pos.z,
								 pos.x + size.x, pos.y + size.y, pos.z + size.z,
								 pos.x + size.x, pos.y + size.y, pos.z + size.z,
								 pos.x, pos.y + size.y, pos.z + size.z,
								 pos.x, pos.y + size.y, pos.z + size.z,
								 pos.x, pos.y + size.y, pos.z,
								 /* top to bottom */
								 pos.x, pos.y, pos.z,
								 pos.x, pos.y + size.y, pos.z,
								 pos.x + size.x, pos.y, pos.z,
								 pos.x + size.x, pos.y + size.y, pos.z,
								 pos.x + size.x, pos.y, pos.z + size.z,
								 pos.x + size.x, pos.y + size.y, pos.z + size.z,
								 pos.x, pos.y, pos.z + size.z,
								 pos.x, pos.y + size.y, pos.z + size.z
							  };
		_vertices.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 3 * 24, vertices );
		_vao.setVertexData( _vertices, 3, GL_FLOAT );
		_vao.setColor( c );
		_numPts = 24;
	}

	inline GLLines::GLLines( const SceneLines& lines ) :
		_vertices( GL_ARRAY_BUFFER ),
		_colors( GL_ARRAY_BUFFER ),
		_numPts( lines.vertexSize() )
	{
		_vertices.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 3 * lines.vertexSize(), lines.vertices() );
		_colors.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 4 * lines.colorSize(), lines.colors() );

		_vao.setVertexData( _vertices, 3, GL_FLOAT );
		_vao.setColorData( _colors, 4, GL_FLOAT );
	}

	inline GLLines::~GLLines()
	{
	}

	inline void GLLines::draw( GLenum mode )
	{
		_vao.draw( mode, 0, _numPts );
	}
}

#endif
