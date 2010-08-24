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

			virtual void draw() const = 0;

			const Recti& rect() const { return _rect; };
			void setRect( const Recti& r ) { _rect = r; };

			void setSize( int w, int h ) { _rect.setSize( w, h ); updated(); };
			void setPosition( int x, int y ) { _rect.setPosition( x, y ); updated(); };

			boost::signal<void ()>  updated;

		protected:
			Recti _rect;
			Color _color;
	};

}

#endif
