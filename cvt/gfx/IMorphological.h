#ifndef CVT_IMORPHOLOGICAL_H
#define CVT_IMORPHOLOGICAL_H

namespace cvt
{
	class Image;

	class IMorphological
	{
		public:
			static void dilate( Image& dst, const Image& src, size_t radius );
			static void erode( Image& dst, const Image& src, size_t radius );
//			static void open( Image& dst, const Image& src, size_t radius );
//			static void close( Image& dst, const Image& src, size_t radius );
		private:
			IMorphological() {}
			IMorphological( const IMorphological& ) {}
	};
}

#endif
