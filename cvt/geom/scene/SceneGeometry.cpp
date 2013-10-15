#include <cvt/geom/scene/SceneGeometry.h>
#include <cvt/geom/scene/SceneMesh.h>
#include <cvt/geom/scene/ScenePoints.h>

namespace cvt {

	std::ostream& operator<<( std::ostream& out, const SceneGeometry& g )
	{
		switch( g.type() ) {
			case SCENEGEOMETRY_MESH:
									 out << ( const SceneMesh& ) g;
									 break;
			case SCENEGEOMETRY_POINTS: out << ( const ScenePoints& ) g;
									 break;
		}
		out << "Material: " << g.material() << std::endl;
		return out;
	}
}
