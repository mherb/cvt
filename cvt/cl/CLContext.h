#ifndef OPENCL_H
#define OPENCL_H
#include <iostream>
#include <map>

#include <cvt/cl/cl.hpp>
#include <cvt/cl/CLFilter.h>
#include <cvt/cl/CLException.h>

namespace cvt {

		class CLFilter;

		class CLContext
		{
			public:
				CLContext( void );
				~CLContext( );

				CLFilter* getFilter( const std::string name ) { return filters[ name ]; };

				const ::cl::Context& getCLContext( )
				{
					return context;
				};

				const ::cl::CommandQueue& getCLQueue( )
				{
					return queue;
				};
				const ::cl::Device& getCLDevice( )
				{
					return device;
				};
				void infoImageFormats( std::ostream& out );

				size_t getComputingUnits( )
				{
					return cunits;
				};
				size_t getMaxWorkGroupSize( )
				{
					return maxworkgroupsize;
				};
				size_t getMaxWorkDimension( size_t d )
				{
					if( maxworkdim.size( ) < d ) return 0;return maxworkdim[ d ];
				};


				static void info( std::ostream& out );

			private:
				CLContext( const CLContext& );
				void loadFilters( void );
				CLFilter* loadFilter( const char* path );

				static const char* getImageFormatChannelOrderString( ::cl::ImageFormat format );
				static const char* getImageFormatChannelTypeString( ::cl::ImageFormat format );

				::cl::Platform platform;
				::cl::Device device;
				::cl::Context context;
				::cl::CommandQueue queue;
				std::map<const std::string, CLFilter*> filters;
				cl_uint cunits;
				size_t maxworkgroupsize;
				std::vector<size_t> maxworkdim;

				static const char* PLUGIN_SUFFIX;
		};

}
#endif
