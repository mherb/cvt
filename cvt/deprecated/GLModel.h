/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLMODEL_H
#define CVT_GLMODEL_H

#include <cvt/geom/Model.h>
#include <cvt/gl/GLBuffer.h>
#include <cvt/gl/GLVertexArray.h>


namespace cvt
{

	/**
	  \ingroup GL
	*/
	class GLModel
	{
		public:
			GLModel();
			GLModel( const Model& mdl );
			~GLModel();

			void setColor( const Color& col );
			void draw() const;
		private:
			GLVertexArray _vao;
			GLBuffer	  _vtxbuf;
			GLBuffer	  _normbuf;
			GLBuffer	  _facebuf;
			size_t		  _numvertices;
			size_t		  _numfaces;
			GLenum		  _mode;
	};

	inline void GLModel::draw() const
	{
		_vao.drawIndirect( _facebuf, GL_UNSIGNED_INT, _mode, _numfaces * 3 );
	}

	inline void GLModel::setColor( const Color& col )
	{
		_vao.setColor( col );
	}

}

#endif
