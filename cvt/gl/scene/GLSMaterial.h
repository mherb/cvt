#ifndef CVT_GLSMATERIAL_H
#define CVT_GLSMATERIAL_H

#include <cvt/gl/scene/GLSNode.h>
#include <cvt/gfx/Color.h>

namespace cvt {
	class GLSMaterial
	{
		public:
			GLSMaterial();
			~GLSMaterial();

			const Color& specularColor() const;
			void		 setSpecularColor( const Color& c );
			const float	 specularIntensity() const;
			void		 setSpecularIntensity( float intensity );

			const Color& diffuseColor() const;
			void		 setDiffuseColor( const Color& c );

			const Color& ambientColor() const;
			void		 setAmbientColor( const Color& c );

		private:
			Color	_ambient;
			Color	_diffuse;
			Color	_specular;
			float	_specularIntensity;
	};
}

#endif
