#include <cvt/gl/scene/GLSBaseModel.h>
#include <cvt/gl/scene/GLSMaterial.h>
#include <cvt/gl/scene/GLSVisitor.h>

namespace cvt {

	void GLSBaseModel::accept( GLSVisitor& visitor )
	{
		visitor.visit( *this );
	}
}
