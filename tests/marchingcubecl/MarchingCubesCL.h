#ifndef CVT_MARCHINGCUBESCL_H
#define CVT_MARCHINGCUBESCL_H

#include <cvt/cl/kernel/MC.h>
#include <cvt/util/Time.h>

namespace cvt {
	class MarchingCubesCL
	{
		public:
			MarchingCubesCL() :
				_clMCsize( _MC_source, "MC_weighted_triangleSizeWNormals" ),
				_clMCextract( _MC_source, "MC_weighted_extractTrianglesNormals" )
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

			void extractTriangles( CLBuffer& outputvtx, CLBuffer& outputnormal, const CLBuffer& buf , size_t width, size_t height, size_t depth, float isolevel = 0.0f )
			{
				unsigned int ret = 0;
				CLBuffer clbuf( &ret, sizeof( unsigned int ) );

				_clMCextract.setArg( 0, outputvtx );
				_clMCextract.setArg( 1, outputnormal );
				_clMCextract.setArg( 2, clbuf );
				_clMCextract.setArg( 3, buf );
				_clMCextract.setArg( 4, ( int ) width );
				_clMCextract.setArg( 5, ( int ) height );
				_clMCextract.setArg( 6, ( int ) depth );
				_clMCextract.setArg( 7, isolevel );

				Time t;
				_clMCextract.runWait( CLNDRange( Math::pad16( width ), Math::pad16( height ), depth ), CLNDRange( 16, 16, 1 ) );
				std::cout << "Extracting tris took "<< t.elapsedMilliSeconds() << " ms" << std::endl;
			}

		private:
			CLKernel _clMCsize;
			CLKernel _clMCextract;
	};
}

#endif
