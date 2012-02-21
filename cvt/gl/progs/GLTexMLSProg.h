/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLPROGTEXMLS_H
#define CVT_GLPROGTEXMLS_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/gl/GLVertexArray.h>
#include <cvt/gl/GLBuffer.h>
#include <cvt/gl/GLTexture.h>

namespace cvt {
	/**
	  \class GLTexMLSProg GLTexMLSProg.h
	  \brief OpenGL program for Moving Least Squares warping.
	*/
	class GLTexMLSProg : private GLProgram
	{
		public:
			GLTexMLSProg();
			~GLTexMLSProg();

			using GLProgram::bind;
			using GLProgram::unbind;
			void  setProjection( const Matrix4f& projection );
			void  setAlpha( float alpha );
			void  setGridSize( size_t gridwidth, size_t gridheight );
			void  setDisplacements( const Vector4f* disp, size_t size );
			void  drawImage( int x, int y, int width, int height, const Image& img );

		private:
			bool _glsl120;
			GLBuffer _vbo;
			GLVertexArray _vao;
			GLTexture _dptex;
			size_t _gridsize;
			Matrix4f _proj;
			GLint _mvploc;
			GLint _texloc;
			GLint _dptexloc;
			GLint _dptexsizeloc;
	};


}


#endif
