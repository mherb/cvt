#ifndef CVTGFXGL_H
#define CVTGFXGL_H

#include <cvt/gui/GFX.h>

namespace cvt {
	class GFXGL : public GFX
	{
		friend class WidgetImplWinGLX11;
		public:
			GFXGL();
			~GFXGL() {};

			virtual void fillRect( const Recti& rect );
			virtual void fillRect( int x, int y, int width, int height );

		private:
			void updateState();
	};
}


#endif
