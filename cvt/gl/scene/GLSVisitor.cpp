
#include <cvt/gl/scene/GLSVisitor.h>

#include <cvt/gl/scene/GLSRenderableGroup.h>
#include <cvt/gl/scene/GLSBaseModel.h>

namespace cvt {

	void GLSRenderVisitor::visit( GLSRenderableGroup& group )
	{
		Matrix4f told = _shader.transformation();
		Matrix4f tnew = told * group.transformation();

		_shader.setTransformation( tnew );
		group.visitChildren( *this );
		_shader.setTransformation( told );
	}

	void GLSRenderVisitor::visit( GLSBaseModel& bmodel )
	{
		Matrix4f told = _shader.transformation();
		Matrix4f tnew = told * bmodel.transformation();

//		std::cout << _shader.transformation() << std::endl;

		_shader.setMaterial( bmodel.material() );
		_shader.bind();
		_shader.setTransformation( tnew, true );
		bmodel.mesh()->draw();
		_shader.unbind();
		_shader.setTransformation( told );
	}

}
