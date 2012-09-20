/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLSLIGHT_H
#define CVT_GLSLIGHT_H

#include <cvt/gl/scene/GLSTransformable.h>
#include <cvt/math/Vector.h>
#include <cvt/gfx/Color.h>

namespace cvt {
	enum GLSLightType {
		GLSLIGHT_POINT,
		GLSLIGHT_SPOT
	};

	class GLSLight : public GLSTransformable
	{
		public:
			GLSLight( GLSLightType ltype );
			~GLSLight();

			GLSLightType	 lightType() const;

			void			 setColor( const Color& c );
			const Color&	 color() const;

			bool			 visible() const { return _visible; }
			void			 setVisible( bool b ) { _visible = b; }

		private:
			GLSLightType _ltype;
			Color		 _color;
			bool		 _visible;
	};

	inline GLSLight::GLSLight( GLSLightType ltype ) :  _ltype( ltype )
	{
	}

	inline GLSLight::~GLSLight()
	{
	}

	inline GLSLightType GLSLight::lightType() const
	{
		return _ltype;
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
