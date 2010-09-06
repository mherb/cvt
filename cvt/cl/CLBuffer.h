#ifndef CLBUFFER_H
#define CLBUFFER_H
#include <CLManager.h>
#include <CLParameter.h>

namespace cvt {
	namespace cl {

		template<typename T>
			class CLBuffer : public CLParameter
		{
			public:
				CLBuffer( CLManager* cl, size_t elements );
				size_t length( )
				{
					return len;
				};
				size_t getSize( )
				{
					return len * sizeof( T );
				};
				std::pair<size_t, void*> getCLArgument( )
				{
					return std::make_pair<size_t, void*>( sizeof( cl_mem ), static_cast<void*>( &clmem ) );
				};
				cl::NDRange getCLNDRange( ) const
				{
					return cl::NDRange( len );
				};
				void read( void* dst, size_t size, size_t offset = 0 )
				{
					cl_int err = cl->getCLQueue( ).enqueueReadBuffer( clbuffer, CL_TRUE, offset, size, dst );
					if( err != CL_SUCCESS )
						throw CLException( __PRETTY_FUNCTION__, err );
				};
				void write( void* src, size_t size, size_t offset = 0 )
				{
					cl_int err = cl->getCLQueue( ).enqueueWriteBuffer( clbuffer, CL_TRUE, offset, size, src );
					if( err != CL_SUCCESS )
						throw CLException( __PRETTY_FUNCTION__, err );
				};

			protected:
				CLBuffer( const CLBuffer& ) {};

			private:
				CLBuffer( );

				CLManager* cl;
				cl_mem clmem;
				cl::Buffer clbuffer;
				size_t len;
		};


#define CLBUFFER_TYPE( a, b ) \
		template < > \
		CLBuffer < a > ::CLBuffer( CLManager * cl, size_t elements ) : CLParameter( b ), len( elements ) \
		{ \
			cl_int err;	\
			this->cl = cl; \
			clbuffer = cl::Buffer( cl->getCLContext( ), CL_MEM_READ_WRITE, getSize( ), NULL, &err ); \
			clmem = clbuffer( ); \
			if( err != CL_SUCCESS )	\
			throw CLException( __PRETTY_FUNCTION__, err );\
		};

		CLBUFFER_TYPE( cl_float, CL_PARAM_BUFFER_FLOAT )
			CLBUFFER_TYPE( cl_float2, CL_PARAM_BUFFER_FLOAT2 )
			CLBUFFER_TYPE( cl_float4, CL_PARAM_BUFFER_FLOAT4 )
			CLBUFFER_TYPE( cl_float8, CL_PARAM_BUFFER_FLOAT8 )
			CLBUFFER_TYPE( cl_float16, CL_PARAM_BUFFER_FLOAT16 )

			typedef CLBuffer<cl_float>   CLBufferFloat;
		typedef CLBuffer<cl_float2>  CLBufferFloat2;
		typedef CLBuffer<cl_float4>  CLBufferFloat4;
		typedef CLBuffer<cl_float8>  CLBufferFloat8;
		typedef CLBuffer<cl_float16> CLBufferFloat16;

	}
}
#endif
