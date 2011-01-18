#ifndef CVT_LABEL_H
#define CVT_LABEL_H

#include <cvt/gui/Widget.h>
#include <cvt/util/Signal.h>

namespace cvt {
	class Label : public Widget
	{
		public:
			Label( const std::string& label );
			~Label();

		private:
			void paintEvent( PaintEvent* e, GFX* g );
			Label( const Label& b );
			std::string _label;

	};
}

#endif
