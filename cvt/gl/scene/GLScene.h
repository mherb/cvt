/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLSCENE_H
#define CVT_GLSCENE_H

#include <cvt/gl/scene/GLSNode.h>

namespace cvt {
	class GLScene : public GLSNode
	{
		public:
			GLScene();
			~GLScene();

			void draw() {}
	};

	inline GLScene::GLScene() : GLSNode( GLSNODE_ROOT )
	{
	}

	inline GLScene::~GLScene()
	{
	}

}

#endif
