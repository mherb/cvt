/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLPOINTS_H
#define CVT_GLPOINTS_H

#include <cvt/gl/GLVertexArray.h>
#include <cvt/geom/scene/ScenePoints.h>

namespace cvt {
	class GLPoints {
		public:
			GLPoints();
			GLPoints( const ScenePoints& pts );
			/*GLPoints( const Vector3f* vertices, const Vector3f* normals, const Vector2f* texcoords, size_t vsize,
				    const unsigned int* indices, size_t isize, MeshType type );*/
			~GLPoints();

                        void setScenePoints( const ScenePoints& pts );
			void draw();

		private:
			GLVertexArray _vao;
			GLBuffer _vertices;
			GLBuffer _colors;
			size_t	 _numPts;
	};

	inline GLPoints::GLPoints() :
		_vertices( GL_ARRAY_BUFFER ),
		_colors( GL_ARRAY_BUFFER ),
		_numPts( 0 )
	{
	}

	inline GLPoints::GLPoints( const ScenePoints& points ) :
		_vertices( GL_ARRAY_BUFFER ),
                _colors( GL_ARRAY_BUFFER )
	{
                setScenePoints( points );
	}

	inline GLPoints::~GLPoints()
	{
	}

        inline void GLPoints::setScenePoints( const ScenePoints& pts )
        {
            _numPts = pts.vertexSize();
            _vertices.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 3 * pts.vertexSize(), pts.vertices() );
            _colors.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 4 * pts.colorSize(), pts.colors() );

            _vao.setVertexData( _vertices, 3, GL_FLOAT );
            _vao.setColorData( _colors, 4, GL_FLOAT );
        }

	inline void GLPoints::draw()
	{
		_vao.draw( GL_POINTS, 0, _numPts );
	}
}

#endif
