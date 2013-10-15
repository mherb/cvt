#ifndef CVT_THRESHOLD_H
#define CVT_THRESHOLD_H

namespace cvt {
	class Image;

	class IThreshold
	{
		public:
			static void threshold( Image& dst, const Image& src, float t );
			static void thresholdAdaptive( Image& dst, const Image& src, const Image& boxfiltered, float t );

		private:
			IThreshold() {}
			IThreshold( const IThreshold& ) {}
	};

}

#endif
