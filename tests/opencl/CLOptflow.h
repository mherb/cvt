#ifndef CLOPTFLOW_H
#define CLOPTFLOW_H

#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLImage.h>

namespace cvt {

	class CLOptflow {
		public:
			CLOptflow();
			~CLOptflow();

			CLImage* updateFlow( const Image& i );

		private:
			void clear( CLImage* img );
			CLImage* biup( CLImage* in, float mul );
			void warp( CLImage* u, CLImage* v, CLImage* px, CLImage* py, CLImage* img1, CLImage* img2, size_t iter );
			void tvl1( CLImage* u, CLImage* v, CLImage* px, CLImage* py, float lambda, float _theta, CLImage* ig2, CLImage* it, CLImage* ix, CLImage* iy, CLImage* _v0, size_t iter );
			CLImage* colorcode( CLImage* in );


			int pyridx;
			CLImage* pyr[ 5 ][ 2 ];
			CLKernel kernelp1;
		    CLKernel kernelp2;
		    CLKernel kernelcopyimg;
		    CLKernel kernelclearimg;
		    CLKernel kernelbidown;
		    CLKernel kernelbiup;
		    CLKernel kernelwarp;
		    CLKernel kernelwarpsub;
		    CLKernel kernelgrad;
			CLKernel kernelth;
			CLKernel kernelcflow;
	};


}


#endif
