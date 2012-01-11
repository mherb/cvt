#ifndef CVT_SCENENODE_H
#define CVT_SCENENODE_H

#include <cvt/util/String.h>

namespace cvt {
	class SceneNode {
		public:
			SceneNode( const String& name );
			~SceneNode();

			const String& name() const;

		private:
			String _name;
	};

	inline SceneNode::SceneNode( const String& name ) : _name( name )
	{
	}

	inline SceneNode::~SceneNode()
	{
	}
}

#endif
