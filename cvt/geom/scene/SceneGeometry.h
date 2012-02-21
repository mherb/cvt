/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_SCENEGEOMETRY_H
#define CVT_SCENEGEOMETRY_H


#include <cvt/geom/scene/SceneSpatial.h>
#include <cvt/geom/Box.h>

namespace cvt {

	class SceneMesh;

	enum SceneGeometryType {
		SCENEGEOMETRY_MESH
	};

	class SceneGeometry : public SceneSpatial
	{
		public:
								SceneGeometry( const String& name, SceneGeometryType type );
			virtual				~SceneGeometry();

			SceneGeometryType	type() const;

			virtual Boxf		boundingBox() const = 0;

		private:
			SceneGeometryType _type;
	};

	inline SceneGeometry::SceneGeometry( const String& name, SceneGeometryType type ) : SceneSpatial( name ), _type( type )
	{
	}

	inline SceneGeometry::~SceneGeometry()
	{
	}

	inline SceneGeometryType SceneGeometry::type() const
	{
		return _type;
	}

	std::ostream& operator<<( std::ostream& out, const SceneGeometry& g );
}

#endif
