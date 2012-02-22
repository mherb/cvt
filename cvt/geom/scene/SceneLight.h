/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_SCENELIGHT_H
#define CVT_SCENELIGHT_H

namespace cvt {
	enum SceneLightType {
		SCENELIGHT_AMBIENT,
		SCENELIGHT_DIRECTIONAL,
		SCENELIGHT_POINT,
		SCENELIGHT_SPOT
	};

	class SceneLight : public SceneSpatial {
		public:
			SceneLight( const String& name, SceneLightType type );
			~SceneLight();

			SceneLightType type() const;

			const Color& ambientColor() const;
			const Color& diffuseColor() const;
			const Color& specularColor() const;

		private:
			SceneLightType _type;
			Color		   _ambient;
			Color		   _diffuse;
			Color		   _specular;
			float		   _intensity;
			float		   _constant;
			float		   _linear;
			float		   _quadratic;
			bool		   _attenuate;
			bool		   _active;

			// for spotlights
			float		   _exponent;
			float		   _angle;
	};

	inline SceneLight::SceneLight( const String& name, SceneLightType type ) : SceneSpatial( name ), _type( type )
	{
	}

	inline SceneLight::~SceneLight()
	{
	}
}

#endif
