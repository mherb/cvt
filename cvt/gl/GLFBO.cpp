#include <cvt/gl/GLFBO.h>
#include <cvt/gl/GFXEngineGLFBO.h>

namespace cvt {
	GFXEngine* GLFBO::gfxEngine()
	{
		return new GFXEngineGLFBO( *this );
	}
}
