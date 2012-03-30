/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/geom/scene/SceneGeometry.h>
#include <cvt/geom/scene/SceneMesh.h>
#include <cvt/geom/scene/ScenePoints.h>

namespace cvt {

	std::ostream& operator<<( std::ostream& out, const SceneGeometry& g )
	{
		switch( g.type() ) {
			case SCENEGEOMETRY_MESH: out << ( const SceneMesh& ) g;
									 break;

			case SCENEGEOMETRY_POINTS: out << ( const ScenePoints& ) g;
									 break;
		}
		return out;
	}
}
