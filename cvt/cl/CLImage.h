#ifndef CLIMAGE_H
#define CLIMAGE_H
#include <cvt/cl/CLManager.h>
#include <cvt/cl/CLParameter.h>

namespace cvt {
	namespace cl {


		class CLImage : public CLParameter
		{
			public:
				CLImage( CLManager* cl );
				CLImage( CLManager* cl, ::cl::ImageFormat format, size_t width, size_t height, void* data = 0, size_t stride = 0 );
				~CLImage( );
				void read( void* data, size_t stride = 0 ) const;
				void write( void* data, size_t stride = 0 );

				std::pair<size_t, void*> getCLArgument( );
				::cl::NDRange getCLNDRange( ) const;

				size_t getWidth( ) const { return width; };
				size_t getHeight( ) const { return height; };
				void copy( const CLImage& src );
				void* map( size_t& stride );
				void unmap( );
				void syncMapRead( );
				void syncMapWrite( );

			protected:
				CLImage( const CLImage& ) {};
				CLImage( );

			private:

				CLManager* cl;
				cl_mem clmem;
				::cl::ImageFormat format;
				size_t width, height;
				::cl::Image2D climage;
				void* mapptr;
				size_t mapstride;
		};

	}
}
#endif
