#ifndef CVT_IFILL_H
#define CVT_IFILL_H

namespace cvt
{
	class Image;
	class Color;

	class IFill {
		public:
			static void fill( Image& dst, const Color& c );

		private:
			IFill() {}
			IFill( const IFill& ) {}
	};

}

#endif
