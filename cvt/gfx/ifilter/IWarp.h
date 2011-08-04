#ifndef CVT_IWARP_H
#define CVT_IWARP_H

#include <cvt/gfx/IFilter.h>
#include <cvt/util/Plugin.h>
#include <cvt/util/PluginManager.h>

namespace cvt {
	class IWarp : public IFilter {
		public:
			IWarp();
			~IWarp();
			void apply( const ParamSet* attribs, IFilterType iftype ) const;

			static void apply( Image& dst, const Image& src, const Image& warp );

			static void warpTunnel( Image& dst, float radius, float cx, float cy );
			static void warpFishEye( Image& idst, float strength, float cx, float cy );
			static void warpUndistort( Image& idst, float k1, float k2, float cx, float cy, float fx, float fy, float k3 = 0, float p1 = 0, float p2 = 0 );

		private:
			static void applyFC1( Image& dst, const Image& src, const Image& warp );
			static void applyFC4( Image& dst, const Image& src, const Image& warp );
			static void applyU8C1( Image& dst, const Image& src, const Image& warp );
			static void applyU8C4( Image& dst, const Image& src, const Image& warp );

			IWarp( const IWarp& t );
	};
}

#endif
