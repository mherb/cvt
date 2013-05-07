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
#include <cvt/geom/scene/Scene.h>
#include <cvt/gl/GLTexture.h>

namespace cvt {
	enum GLSMaterialFeatures {
		GL_SCENEMATERIAL_NONE			= ( 1 << 0 ),
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
			GLSMaterial( const String& name );
			GLSMaterial( const SceneMaterial& mat, const Scene& scene );
			~GLSMaterial();

			const Color& specularColor() const;
			void		 setSpecularColor( const Color& c );

			float		 shininess() const;
			void		 setShininess( float shininess );

			const Color& diffuseColor() const;
			void		 setDiffuseColor( const Color& c );

			const Color& ambientColor() const;
			void		 setAmbientColor( const Color& c );

			const String& name() const;

			GLTexture*   diffuseMap() const { return _diffuseMap; }


			GLSMaterialFlags flags() const { return _flags; }
			GLSMaterialFlags& flags() { return _flags; }

		private:
			String			 _name;
			GLSMaterialFlags _flags;
			Color			 _ambient;
			Color		     _diffuse;
			Color			 _specular;
			float			 _shininess;
			GLTexture*		 _ambientMap;
			GLTexture*		 _diffuseMap;
			GLTexture*		 _specMap;
			GLTexture*		 _normalMap;
	};

	inline GLSMaterial::GLSMaterial( const String& name ) : _name( name ), _flags( GL_SCENEMATERIAL_NONE ), _shininess( 1.0f ), _ambientMap( NULL ), _diffuseMap( NULL ), _specMap( NULL ), _normalMap( NULL )
	{
	}

	inline GLSMaterial::GLSMaterial( const SceneMaterial& mat, const Scene& scene ) : _name( mat.name() ), _flags( GL_SCENEMATERIAL_NONE ), _shininess( 1.0f ), _ambientMap( NULL ), _diffuseMap( NULL ), _specMap( NULL ), _normalMap( NULL )
	{
		const SceneTexture* tex;

		if( mat.flags() & SCENEMATERIAL_AMBIENT ) {
			_flags |= GL_SCENEMATERIAL_AMBIENT;
			_ambient = mat.ambientColor();
		}

		if( mat.flags() & SCENEMATERIAL_DIFFUSE ) {
			_flags |= GL_SCENEMATERIAL_DIFFUSE;
			_diffuse = mat.diffuseColor();
		}

		if( mat.flags() & SCENEMATERIAL_SPECULAR ) {
			_flags |= GL_SCENEMATERIAL_SPECULAR;
			_specular = mat.specularColor();
			_shininess = mat.shininess();
		}

		if( mat.flags() & SCENEMATERIAL_AMBIENTMAP && ( tex = scene.texture( mat.ambientMap() ) ) != NULL ) {
			_flags |= GL_SCENEMATERIAL_AMBIENT_MAP;
			_ambientMap = new GLTexture( tex->image()  );
		}

		if( mat.flags() & SCENEMATERIAL_DIFFUSEMAP && ( tex = scene.texture( mat.diffuseMap() ) ) != NULL) {
			_flags |= GL_SCENEMATERIAL_DIFFUSE_MAP;
			_diffuseMap = new GLTexture( tex->image() );
		}

		if( mat.flags() & SCENEMATERIAL_SPECULARMAP && ( tex = scene.texture( mat.specularMap() ) ) != NULL) {
			_flags |= GL_SCENEMATERIAL_SPECULAR_MAP;
			_specMap = new GLTexture( tex->image() );
		}

		if( mat.flags() & SCENEMATERIAL_NORMALMAP && ( tex = scene.texture( mat.normalMap() ) ) != NULL) {
			_flags |= GL_SCENEMATERIAL_NORMALMAP;
			_normalMap = new GLTexture( tex->image() );
		}

	}


	inline GLSMaterial::~GLSMaterial()
	{
		if( _ambientMap ) delete _ambientMap;
		if( _diffuseMap ) delete _diffuseMap;
		if( _specMap )    delete _specMap;
		if( _normalMap )  delete _normalMap;
	}

	inline const Color& GLSMaterial::specularColor() const
	{
		return _specular;
	}

	inline void GLSMaterial::setSpecularColor( const Color& c )
	{
		_specular = c;
		_flags |= GL_SCENEMATERIAL_SPECULAR;
	}

	inline float GLSMaterial::shininess() const
	{
		return _shininess;
	}

	inline void GLSMaterial::setShininess( float shininess )
	{
		_shininess = shininess;
	}

	inline const Color& GLSMaterial::diffuseColor() const
	{
		return _diffuse;
	}

	inline void GLSMaterial::setDiffuseColor( const Color& c )
	{
		_diffuse = c;
		_flags |= GL_SCENEMATERIAL_DIFFUSE;
	}

	inline const Color& GLSMaterial::ambientColor() const
	{
		return _ambient;
	}

	inline void GLSMaterial::setAmbientColor( const Color& c )
	{
		_ambient = c;
		_flags |= GL_SCENEMATERIAL_AMBIENT;
	}

	inline const String& GLSMaterial::name() const
	{
		return _name;
	}

}

#endif
