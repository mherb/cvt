#ifndef CVT_GLSSHADERFACTORY_H
#define CVT_GLSSHADERFACTORY_H

#include <cvt/gl/GLProgram.h>
#include <cvt/gl/scene/GLSMaterial.h>
#include <cvt/gl/scene/GLSLight.h>

namespace cvt {

	class GLSShaderProgram : GLProgram {
		public:
			GLSShaderProgram();
			~GLSShaderProgram();

			void setLight( int index, const GLSLight* light );
			void setMaterial( const GLSMaterial* mat );
			void setProjection( const Matrix4f& proj, const Matrix4f& modelview );
	};


	class GLSShaderFactory {
		public:
			GLSShaderFactory()
			~GLSShaderFactory();

			GLSShaderProgram* programForMaterial( const GLSMaterial* mat );

		private:
	};
}

#endif
