/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef OPENCL_H
#define OPENCL_H
#include <iostream>
#include <map>

#include <cvt/cl/cl.hpp>
#include <cvt/cl/CLException.h>

namespace cvt {
		class CLContext
		{
			public:
				CLContext( void );
				~CLContext();

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

				void makeCurrent()
				{
					_current = this;
				}

				static CLContext* getCurrent()
				{
					return _current;
				}

				void infoImageFormats( std::ostream& out );
				static void info( std::ostream& out );


			private:
				CLContext( const CLContext& );

				static const char* getImageFormatChannelOrderString( ::cl::ImageFormat format );
				static const char* getImageFormatChannelTypeString( ::cl::ImageFormat format );

				::cl::Platform platform;
				::cl::Device device;
				::cl::Context context;
				::cl::CommandQueue queue;
				cl_uint cunits;
				size_t maxworkgroupsize;
				std::vector<size_t> maxworkdim;
				static CLContext* _current;
		};

}
#endif
