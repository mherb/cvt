#ifndef CVT_GLSRENDERABLEGROUP_H
#define CVT_GLSRENDERABLEGROUP_H

#include <cvt/gl/scene/GLSRenderable.h>
#include <cvt/container/List.h>

namespace cvt {
	class GLSVisitor;

	class GLSRenderableGroup : public GLSRenderable {
		public:
			GLSRenderableGroup();
			~GLSRenderableGroup();

			void add( GLSRenderable* renderable );
			void remove( GLSRenderable* renderable );

			void accept( GLSVisitor& visitor );
			void visitChildren( GLSVisitor& visitor );
		private:
			List<GLSRenderable*> _renderables;
	};

	inline GLSRenderableGroup::GLSRenderableGroup()
	{
	}


	inline GLSRenderableGroup::~GLSRenderableGroup()
	{
	}

	inline void GLSRenderableGroup::add( GLSRenderable* renderable )
	{
		_renderables.append( renderable );
	}

	inline void GLSRenderableGroup::remove( GLSRenderable* renderable )
	{
		List<GLSRenderable*>::Iterator it;
		it = _renderables.find( renderable );
		_renderables.remove( it );
	}

}

#endif
