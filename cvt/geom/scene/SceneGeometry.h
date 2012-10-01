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
#include <cvt/util/String.h>
#include <cvt/geom/Box.h>

namespace cvt {

	class SceneMesh;

	enum SceneGeometryType {
		SCENEGEOMETRY_MESH,
		SCENEGEOMETRY_POINTS
	};

	class SceneGeometry : public SceneSpatial
	{
		public:
								SceneGeometry( const String& name, SceneGeometryType type );
			virtual				~SceneGeometry();

			SceneGeometryType	type() const;

			virtual Boxf		boundingBox() const = 0;
			const String&		material() const;
			void				setMaterial( const String& name );

		private:
			SceneGeometryType	_type;
			String				_material;
	};

	inline SceneGeometry::SceneGeometry( const String& name, SceneGeometryType type ) :
		SceneSpatial( name ),
		_type( type )
	{
	}

	inline SceneGeometry::~SceneGeometry()
	{
	}

	inline const String& SceneGeometry::material() const
	{
		return _material;
	}

	inline void	SceneGeometry::setMaterial( const String& name )
	{
		_material = name;
	}


	inline SceneGeometryType SceneGeometry::type() const
	{
		return _type;
	}

	std::ostream& operator<<( std::ostream& out, const SceneGeometry& g );
}

#endif
