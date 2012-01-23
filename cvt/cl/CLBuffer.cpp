#include <cvt/cl/CLBuffer.h>
#include <cvt/cl/CLContext.h>

namespace cvt
{
	CLBuffer::CLBuffer( const CLContext& context, size_t size, cl_mem_flags flags , void* host_ptr ) : _size( size )
	{
		cl_int err;
		_object = ::clCreateBuffer( context, flags, size, host_ptr, &err );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	CLBuffer::CLBuffer( size_t size, cl_mem_flags flags ) : _size( size )
	{
		cl_int err;
		_object = ::clCreateBuffer( *CL::defaultContext(), flags, size, NULL, &err );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	CLBuffer::CLBuffer( void* data, size_t size, cl_mem_flags flags ) : _size( size )
	{
		cl_int err;
		_object = ::clCreateBuffer( *CL::defaultContext(), flags | CL_MEM_COPY_HOST_PTR, size, data, &err );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	CLContext CLBuffer::context() const
	{
		return CLContext( _context() );
	}

	/**
	  Map the CLBuffer object memory in the host address-space for reading/writing using the default command-queue.
	  \param stride the stride of the mapped image.
	  \return pointer to the mapped memory.
	  */
	void* CLBuffer::map()
	{
		 return CL::defaultQueue()->enqueueMapBuffer( *this, CL_MAP_READ | CL_MAP_WRITE, _size );
	}

	/**
	  Map the CLBuffer object memory in the host address-space for reading using the default command-queue.
	  \param stride the stride of the mapped image.
	  \return pointer to the mapped memory.
	  */
	const void* CLBuffer::map() const
	{
		 return ( const void* ) CL::defaultQueue()->enqueueMapBuffer( *this, CL_MAP_READ, _size );
	}

	/**
	  Unmap the previously mapped CLBuffer object memory using the default command-queue.
	  \param ptr the previously mapped data-pointer.
	  */
	void CLBuffer::unmap( const void* ptr ) const
	{
		CL::defaultQueue()->enqueueUnmap( *( ( CLMemory* )this ), ptr );
	}
}
