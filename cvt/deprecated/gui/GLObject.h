/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef GLOBJECT_H
#define GLOBJECT_H

#include "util/Rect.h"
#include "gfx/Color.h"

#include <GL/gl.h>

#include "signalslib.hpp"

namespace cvt {

	class GLObject {

		public:
			GLObject() : _rect( 0, 0, 1, 1 ) , _color( 1.0f, 1.0f ) {};
			virtual ~GLObject() {};

			virtual void draw() const;

			const Recti& rect() const { return _rect; };
			void setRect( const Recti& r ) { _rect = r; };

			void setSize( int w, int h ) { _rect.setSize( w, h ); updated(); };
			void setPosition( int x, int y ) { _rect.setPosition( x, y ); updated(); };

			boost::signal<void ()>  updated;

		protected:
			Recti _rect;
			Color _color;
	};

	inline void GLObject::draw() const
	{
		glColor4f( _color.red(), _color.green(), _color.blue(), _color.alpha() );
		glBegin( GL_QUADS );
			glVertex2i( _rect.x, _rect.y );
			glVertex2i( _rect.x + _rect.width, _rect.y );
			glVertex2i( _rect.x + _rect.width, _rect.y + _rect.height );
			glVertex2i( _rect.x, _rect.y + _rect.height );
		glEnd();
	}

}

#endif
