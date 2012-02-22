/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_SCENENODE_H
#define CVT_SCENENODE_H

#include <cvt/util/String.h>

namespace cvt {
	class SceneNode {
		public:
			SceneNode( const String& name );
			virtual ~SceneNode();

			const String& name() const;

		protected:
			String _name;
	};

	inline SceneNode::SceneNode( const String& name ) : _name( name )
	{
	}

	inline SceneNode::~SceneNode()
	{
	}

	inline const String& SceneNode::name() const
	{
		return _name;
	}
}

#endif
