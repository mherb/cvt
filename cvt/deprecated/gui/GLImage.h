#ifndef GLIMAGE_H
#define GLIMAGE_H

#include "gui/GLObject.h"
#include "gfx/Image.h"

namespace cvt {
	class GLImage : public GLObject
	{
		public:
			GLImage();
			GLImage( const Image& i );
			virtual ~GLImage();
			virtual void draw() const;
			void setImage( const Image& i );

		private:
			GLuint _tex;
			float  _iw, _ih;
	};
}

#endif
