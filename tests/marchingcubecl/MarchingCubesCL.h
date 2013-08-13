#ifndef CVT_MARCHINGCUBESCL_H
#define CVT_MARCHINGCUBESCL_H

#include <cvt/cl/kernel/MC.h>
#include <cvt/util/Time.h>

namespace cvt {
	class MarchingCubesCL
	{
		public:
			MarchingCubesCL() : _clMCsize( _MC_source, "MC_weighted_triangleSize" )
			{
			}

			size_t triangleSize( const CLBuffer& buf, size_t width, size_t height, size_t depth, float isolevel = 0.0f )
			{
				unsigned int ret = 0;
				CLBuffer clbuf( &ret, sizeof( unsigned int ) );

				_clMCsize.setArg( 0, clbuf );
				_clMCsize.setArg( 1, buf );
				_clMCsize.setArg( 2, ( int ) width );
				_clMCsize.setArg( 3, ( int ) height );
				_clMCsize.setArg( 4, ( int ) depth );
				_clMCsize.setArg( 5, isolevel );

				Time t;
				_clMCsize.runWait( CLNDRange( Math::pad16( width ), Math::pad16( height ), depth ), CLNDRange( 16, 16, 1 ) );
				std::cout << "Counting tris took "<< t.elapsedMilliSeconds() << " ms" << std::endl;

				clbuf.read( &ret );
				return ret;
			}

		private:
			CLKernel _clMCsize;
	};
}

#endif
