/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_SCENE_H
#define CVT_SCENE_H

#include <cvt/geom/scene/SceneMesh.h>
#include <cvt/geom/scene/SceneMaterial.h>
#include <cvt/geom/scene/SceneTexture.h>

namespace cvt {

	class SceneLoader;

	class Scene {
		public:
			Scene();
			~Scene();

			void					clear();

			SceneGeometry*			geometry( size_t index );
			const SceneGeometry*	geometry( size_t index ) const;
			size_t					geometrySize() const;
			void					addGeometry( SceneGeometry* geometry );

			SceneMaterial*			material( size_t index );
			const SceneMaterial*	material( size_t index ) const;
			size_t					materialSize() const;
			void					addMaterial( SceneMaterial* geometry );

			SceneTexture*			texture( size_t index );
			const SceneTexture*		texture( size_t index ) const;
			size_t					textureSize() const;
			void					addTexture( SceneTexture* tex );

			void					load( const String& path, SceneLoader* loader = NULL );

		private:
			std::vector<SceneGeometry*> _geometries;
			std::vector<SceneMaterial*> _materials;
			std::vector<SceneTexture*>	_textures;
	};

	inline Scene::Scene()
	{
	}

	inline Scene::~Scene()
	{
		clear();
	}

	inline void Scene::clear()
	{
		for( std::vector<SceneGeometry*>::iterator it = _geometries.begin(); it != _geometries.end(); ++it ) {
			delete *it;
		}
		for( std::vector<SceneMaterial*>::iterator it = _materials.begin(); it != _materials.end(); ++it ) {
			delete *it;
		}

		_geometries.clear();
		_materials.clear();
	}

	inline SceneGeometry* Scene::geometry( size_t index )
	{
		return _geometries[ index ];
	}

	inline const SceneGeometry* Scene::geometry( size_t index ) const
	{
		return _geometries[ index ];
	}


	inline size_t Scene::geometrySize() const
	{
		return _geometries.size();
	}

	inline void Scene::addGeometry( SceneGeometry* geometry )
	{
		_geometries.push_back( geometry );
	}

	inline SceneMaterial* Scene::material( size_t index )
	{
		return _materials[ index ];
	}

	inline const SceneMaterial* Scene::material( size_t index ) const
	{
		return _materials[ index ];
	}


	inline size_t Scene::materialSize() const
	{
		return _materials.size();
	}

	inline void Scene::addMaterial( SceneMaterial* mat )
	{
		_materials.push_back( mat );
	}

	inline SceneTexture* Scene::texture( size_t index )
	{
		return _textures[ index ];
	}

	inline const SceneTexture* Scene::texture( size_t index ) const
	{
		return _textures[ index ];
	}

	inline size_t Scene::textureSize() const
	{
		return _textures.size();
	}

	inline void Scene::addTexture( SceneTexture* tex )
	{
		_textures.push_back( tex );
	}

	inline std::ostream& operator<<( std::ostream& out, const Scene& s )
	{
		out << "Scene:";
		out << "\n\tGeometry: " << s.geometrySize() << "\n";
		for( size_t i = 0; i < s.geometrySize(); i++ )
			out << *s.geometry( i );
		out << "\n\tMaterials: " << s.materialSize() << "\n";
		for( size_t i = 0; i < s.materialSize(); i++ )
			out << *s.material( i );
		return out;
	}
}

#endif
