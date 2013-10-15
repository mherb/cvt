#ifndef CVT_GLSVISITOR_H
#define CVT_GLSVISITOR_H

#include <cvt/gl/scene/GLSShader.h>

namespace cvt {
	class GLSRenderableGroup;
	class GLSBaseModel;

	class GLSVisitor {
		public:
			GLSVisitor() {}
			virtual ~GLSVisitor() {}

			virtual void visit( GLSRenderableGroup& group ) = 0;
			virtual void visit( GLSBaseModel& bmodel ) = 0;
	};

	class GLSRenderVisitor : public GLSVisitor
	{
		public:
			GLSRenderVisitor( GLSShader& shader ) : _shader( shader ) {}
			~GLSRenderVisitor() {}

			void visit( GLSRenderableGroup& group );
			void visit( GLSBaseModel& bmodel );

		private:
			GLSShader& _shader;
	};

}

#endif
