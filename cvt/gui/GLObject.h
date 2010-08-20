#ifndef GLOBJECT_H
#define GLOBJECT_H

#include <cvt/util/Rect.h>

#include <opengl/gl.h>

namespace cvt {

	class GLObject {
		public:
			virtual void draw() const;

			const Recti& rect() const { return _rect; };
			void setRect( const Recti& r ) { _rect = r; };

			void setSize( int w, int h ) { _rect.setSize( w, h ); };
			void setPosition( int x, int y ) { _rect.setPosition( x, y ); };

		protected:
			Recti _rect;
	};

}

#endif
