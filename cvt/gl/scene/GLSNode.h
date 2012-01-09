#ifndef CVT_GLSNODE_H
#define CVT_GLSNODE_H

#include <vector>

namespace cvt {
	enum GLSNodeType {
		GLSNODE_ROOT,
		GLSNODE_CAMERA,
		GLSNODE_TRANSFORMATION,
		GLSNODE_LIGHT,
		GLSNODE_MESH,
		GLSNODE_SHADER,
		GLSNODE_MATERIAL,
		GLSNODE_TEXTURE,
		GLSNODE_RENDERABLE
	};

	class GLSNode
	{
		public:
			GLSNode( GLSNodeType type );
			virtual ~GLSNode();

			GLSNodeType type() const;
			GLSNode*	child( size_t i );
			GLSNode*	operator[]( size_t i );
			size_t		childSize() const;

		private:
			GLSNodeType _type;
			std::vector<GLSNode*> _children;
	};

	inline GLSNode::GLSNode( GLSNodeType type ) : _type( type )
	{
	}

	inline virtual GLSNode::~GLSNode()
	{
	}

	inline GLSNodeType GLSNode::type() const
	{
		return _type;
	}

	inline GLSNode* GLSNode::child( size_t i )
	{
		return _children[ i ];
	}

	inline GLSNode* GLSNode::operator[]( size_t i )
	{
		return _children[ i ];
	}

	inline size_t GLSNode::childSize() const
	{
		return _children.size();
	}
}

#endif
