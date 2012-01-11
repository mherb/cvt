#ifndef CVT_SCENEMATERIAL_H
#define CVT_SCENEMATERIAL_H

#include <cvt/geom/scene/SceneNode.h>

namespace cvt {
	enum SceneMaterialFlags {
		SCENEMATERIAL_EMIT,
		SCENEMATERIAL_AMBIENT,
		SCENEMATERIAL_DIFFUSE,
		SCENEMATERIAL_SPECULAR,
		SCENEMATERIAL_TEXTURE, // if diffuse and ambient are identical
		SCENEMATERIAL_DIFFUSEMAP,
		SCENEMATERIAL_AMBIENTMAP,
		SCENEMATERIAL_SPECULARMAP
		SCENEMATERIAL_ALPHAMAP, // binary map resp alpha map
		SCENEMATERIAL_NORMALMAP,
	};

	CVT_ENUM_TO_FLAGS( SceneMaterialFlags, SceneMaterialFeatures )

	class SceneMaterial : public SceneNode {
		public:
			SceneMaterial( const String& name );
			~SceneMaterial();

		private:
			Color _emit;
			Color _ambient;
			Color _diffuse;
			Color _specular;
			Color _reflective;
			float _shininess;
			float _reflectivity;
			float _transparency;
			float _refraction;

			SceneMaterialFeatures _features;
	};

	inline SceneMaterial::SceneMaterial( const String& name ) : SceneNode( name )
	{
	}

	inline SceneMaterial::~SceneMaterial()
	{
	}

}

#endif
