/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */

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

		std::cout << "DRAW !" << std::endl;

		_shader.bind();
		_shader.setTransformation( tnew, true );
		bmodel.mesh()->draw();
		_shader.unbind();
		_shader.setTransformation( told );
	}

}
