#ifndef CLOPTFLOW_H
#define CLOPTFLOW_H

#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/gfx/Image.h>

namespace cvt {

	class CLOptflow {
		public:
			CLOptflow();
			~CLOptflow();

			Image* updateFlow( const Image& i );

		private:
			void clear( Image* img );
			Image* biup( Image* in, float mul );
			void warp( Image* u, Image* v, Image* px, Image* py, Image* img1, Image* img2, size_t iter, float level );
			void tvl1( Image* u, Image* v, Image* px, Image* py, float lambda, float _theta, Image* ig2, Image* it, Image* ix, Image* iy, Image* _v0, size_t iter );
			Image* colorcode( Image* in, Image* bg );
			void showColorCode( const char* name, Image* i, Image* bg );


			int pyridx;
			Image* pyr[ 5 ][ 2 ];
			CLKernel kernelp1;
		    CLKernel kernelp2;
		    CLKernel kernelcopyimg;
		    CLKernel kernelclearimg;
		    CLKernel kernelbidown;
		    CLKernel kernelbiup;
		    CLKernel kernelwarp;
		    CLKernel kernelwarpsub;
		    CLKernel kernelgrad;
		    CLKernel kernelgradxy;
			CLKernel kernelth;
			CLKernel kernelcflow;
			CLKernel kernelmed;
	};


}


#endif
