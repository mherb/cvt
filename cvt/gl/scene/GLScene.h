#ifndef CVT_GLSCENE_H
#define CVT_GLSCENE_H

#include <cvt/gl/scene/GLSNode.h>

namespace cvt {
	class GLScene : public GLSNode
	{
		public:
			GLScene();
			~GLScene();
	};

	inline GLScene::GLScene() : GLSNode( GLSNODE_ROOT )
	{
	}

	inline GLScene::~GLScene()
	{
	}
}

#endif
