#ifndef CVT_GLDRAWMODELPROG_H
#define CVT_GLDRAWMODELPROG_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/gl/GLModel.h>
#include <cvt/math/Matrix.h>
#include <cvt/geom/Rect.h>

namespace cvt {
	class GLDrawModelProg : private GLProgram
	{
		public:
			GLDrawModelProg();
			~GLDrawModelProg();

			using GLProgram::bind;
			using GLProgram::unbind;
			void setLightPosition( const Vector3f& position );
			void setProjection( const Matrix4f& projection, const Matrix4f& modelview );
			void drawModel( const GLModel& mdl ) const;

		private:
			GLint _mvploc;
			GLint _normmloc;
			GLint _lightposloc;
	};

	inline void GLDrawModelProg::drawModel( const GLModel& mdl ) const
	{
		mdl.draw();
	}

}

#endif
