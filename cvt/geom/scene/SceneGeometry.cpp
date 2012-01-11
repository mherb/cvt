#include <cvt/geom/scene/SceneGeometry.h>
#include <cvt/geom/scene/SceneMesh.h>

namespace cvt {

	std::ostream& operator<<( std::ostream& out, const SceneGeometry& g )
	{
		switch( g.type() ) {
			case SCENEGEOMETRY_MESH: out << ( const SceneMesh& ) g;
									 break;
		}
		return out;
	}
}
