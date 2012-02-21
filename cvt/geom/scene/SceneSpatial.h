/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_SCENESPATIAL_H
#define CVT_SCENESPATIAL_H

#include <cvt/geom/scene/SceneNode.h>
#include <cvt/math/Matrix.h>

namespace cvt {
	class SceneSpatial : public SceneNode {
		public:
							SceneSpatial( const String& name );
		   virtual			~SceneSpatial();

		   const Matrix4f&	transformation() const { return _transform; }
		   void				setTransformation( const Matrix4f& t ) { _transform = t; }

		protected:
		   Matrix4f _transform;
	};

	inline SceneSpatial::SceneSpatial( const String& name ) : SceneNode( name )
	{
		_transform.setIdentity();
	}

	inline SceneSpatial::~SceneSpatial()
	{
	}
}

#endif
