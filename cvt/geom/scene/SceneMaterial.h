#ifndef CVT_SCENEMATERIAL_H
#define CVT_SCENEMATERIAL_H

#include <cvt/geom/scene/SceneNode.h>

namespace cvt {
	enum SceneMaterialFlags {
		SCENEMATERIAL_EMIT,
		SCENEMATERIAL_AMBIENT,
		SCENEMATERIAL_DIFFUSE,
		SCENEMATERIAL_SPECULAR,
		SCENEMATERIAL_TEXTURE,
		SCENEMATERIAL_BUMPMAP,
		SCENEMATERIAL_SPECULARMAP
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
			float _shininess;

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
