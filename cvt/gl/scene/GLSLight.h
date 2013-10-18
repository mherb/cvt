/*
   The MIT License (MIT)

   Copyright (c) 2011 - 2013, Philipp Heise and Sebastian Klose

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
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
//			GLTexture*	 _shadowmap;
	};

	inline GLSLight::GLSLight( GLSLightType ltype ) :  _ltype( ltype )
	{
	}

	inline GLSLight::~GLSLight()
	{
//		if( _shadowmap )
//			delete _shadowmap;
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
