#ifndef CVT_GLSRENDERABLE_H
#define CVT_GLSRENDERABLE_H

#include <cvt/gl/scene/GLSTransformable.h>

namespace cvt {
	class GLSVisitor;

	class GLSRenderable : public GLSTransformable {
		public:
			virtual ~GLSRenderable() {}
			virtual void accept( GLSVisitor& visitor ) = 0;
	};
}

#endif
