#ifndef CVT_GLSLIGHT_H
#define CVT_GLSLIGHT_H

#include <cvt/gl/scene/GLSNode.h>
#include <cvt/math/Vector.h>
#include <cvt/gfx/Color.h>

namespace cvt {
	enum GLSLightType {
		GLSLIGHT_POINT,
		GLSLIGHT_SPOT
	};

	class GLSLight : public GLSNode
	{
		public:
			GLSLight( GLSLightType ltype );
			~GLSLight();

			GLSLightType	 lightType() const;
			void			 setPosition( const Vector3f& pos );
			const Vector3f&	 position() const;

			void			 setColor( const Color& c );
			const Color&	 color() const;

		private:
			GLSLightType _ltype;
			Vector3f	 _position;
			Color		 _color;
	};

	inline GLSLight::GLSLight( GLSLightType ltype ) : GLSNode( GLSNODE_LIGHT ), _ltype( ltype )
	{
	}

	inline GLSLight::~GLSLight()
	{
	}

	inline GLSLightType GLSLight::lightType() const
	{
		return _ltype;
	}

	inline const Vector3f& GLSLight::position() const
	{
		return _position;
	}

	inline void GLSLight::setPosition( const Vector3f& pos )
	{
		_position = pos;
	}

	inline const Color& GLSLight::color() const
	{
		return _color;
	}

	inline void GLSLight::setColor( const Color& c )
	{
		_color = c;
	}

};

#endif
