#include <cvt/gl/scene/GLSRenderableGroup.h>
#include <cvt/gl/scene/GLSVisitor.h>

namespace cvt {

	void GLSRenderableGroup::visitChildren( GLSVisitor& visitor )
	{
		List<GLSRenderable*>::Iterator end = _renderables.end();
		for( List<GLSRenderable*>::Iterator it = _renderables.begin(); it != end; ++it )
			( *it )->accept( visitor );
	}

	void GLSRenderableGroup::accept( GLSVisitor& visitor )
	{
		visitor.visit( *this );
	}

}
