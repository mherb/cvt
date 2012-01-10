#ifndef CVT_SCENE_H
#define CVT_SCENE_H

#include <cvt/geom/Mesh.h>

namespace cvt {

	class SceneLoader;

	class Scene {
		public:
			Scene();
			~Scene();

			void	clear();
			Mesh*	mesh( size_t index );
			const Mesh*	mesh( size_t index ) const;
			size_t	meshSize() const;
			void	addMesh( Mesh* mesh );

			void	load( const String& path, SceneLoader* loader = NULL );

		private:
			std::vector<Mesh*> _meshes;
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
		for( std::vector<Mesh*>::iterator it = _meshes.begin(); it != _meshes.end(); ++it ) {
			delete *it;
		}
		_meshes.clear();
	}

	inline Mesh* Scene::mesh( size_t index )
	{
		return _meshes[ index ];
	}

	inline const Mesh* Scene::mesh( size_t index ) const
	{
		return _meshes[ index ];
	}


	inline size_t Scene::meshSize() const
	{
		return _meshes.size();
	}

	inline void Scene::addMesh( Mesh* mesh )
	{
		_meshes.push_back( mesh );
	}

	inline std::ostream& operator<<( std::ostream& out, const Scene& s )
	{
		out << "Scene:";
		out << "\n\tMeshes " << s.meshSize() << "\n";
		for( size_t i = 0; i < s.meshSize(); i++ )
			out << *s.mesh( i );
		return out;
	}
}

#endif
