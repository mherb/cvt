/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLSMATERIAL_H
#define CVT_GLSMATERIAL_H

#include <cvt/gfx/Color.h>
#include <cvt/gl/GLTexture.h>

namespace cvt {
	enum GLSMaterialFeatures {
		GL_SCENEMATERIAL_AMBIENT		= ( 1 << 1 ), // rgba ambient
		GL_SCENEMATERIAL_DIFFUSE		= ( 1 << 2 ),
		GL_SCENEMATERIAL_SPECULAR		= ( 1 << 3 ),
		GL_SCENEMATERIAL_DIFFUSE_MAP	= ( 1 << 4 ),
		GL_SCENEMATERIAL_AMBIENT_MAP	= ( 1 << 5 ),
		GL_SCENEMATERIAL_SPECULAR_MAP	= ( 1 << 6 ), // rgb = color, a = shininess
		GL_SCENEMATERIAL_NORMALMAP		= ( 1 << 7 )
	};

	CVT_ENUM_TO_FLAGS( GLSMaterialFeatures, GLSMaterialFlags )



	class GLSMaterial
	{
		public:
			GLSMaterial();
			~GLSMaterial();

			const Color& specularColor() const;
			void		 setSpecularColor( const Color& c );
			float		 specularIntensity() const;
			void		 setSpecularIntensity( float intensity );

			const Color& diffuseColor() const;
			void		 setDiffuseColor( const Color& c );

			const Color& ambientColor() const;
			void		 setAmbientColor( const Color& c );


			GLSMaterialFlags flags() const { return _flags; }

		private:
			GLSMaterialFlags _flags;
			Color	_ambient;
			Color	_diffuse;
			Color	_specular;
			float	_shininess;;
			GLTexture* _ambientMap;
			GLTexture* _diffuseMap;
			GLTexture* _specMap;
			GLTexture* _normalMap;
	};

	inline GLSMaterial::GLSMaterial()
	{
	}


	inline GLSMaterial::~GLSMaterial()
	{
	}


}

#endif
