#ifndef CVT_SCENE_H
#define CVT_SCENE_H

#include <cvt/geom/scene/SceneMesh.h>

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

			void					load( const String& path, SceneLoader* loader = NULL );

		private:
			std::vector<SceneGeometry*> _geometries;
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
		_geometries.clear();
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

	inline std::ostream& operator<<( std::ostream& out, const Scene& s )
	{
		out << "Scene:";
		out << "\n\tGeometry: " << s.geometrySize() << "\n";
		for( size_t i = 0; i < s.geometrySize(); i++ )
			out << *s.geometry( i );
		return out;
	}
}

#endif
