#include <cvt/cl/CLKernel.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/ImageAllocatorCL.h>
#include <cvt/gfx/Color.h>

namespace cvt {

	void CLKernel::build( const char* name, const char* src, size_t len, std::string& buildinfo )
	{
		cl_int err;
		cl::Program::Sources source( 1, std::make_pair<const char*, size_t>( src, len ) );
		std::vector<cl::Device> devices;

		_name = name;
		_cl = CLContext::getCurrent();

		_prog = cl::Program( _cl->getCLContext( ), source, &err );
		if( err != CL_SUCCESS ) {
			throw CLException( err );
		}

		devices.push_back( _cl->getCLDevice( ) );
		err = _prog.build( devices, "-w -cl-mad-enable" );
		buildinfo = _prog.getBuildInfo<CL_PROGRAM_BUILD_LOG>( _cl->getCLDevice( ) );
		if( err != CL_SUCCESS ) {
			std::cout << buildinfo << std::endl;
			throw CLException( err, buildinfo );
		}

		_kernel = cl::Kernel( _prog, name, &err );
		if( err != CL_SUCCESS ) {
			throw CLException( err );
		}
	}
	
	void CLKernel::setArg( size_t n, float f )
	{
		_kernel.setArg( ( cl_uint ) n, sizeof( cl_float ), ( void* ) &f );
	}
	
	void CLKernel::setArg( size_t n, const Color & c )
	{
		_kernel.setArg( ( cl_uint ) n, sizeof( cl_float4 ), ( void* ) c.data() );
	}
	
	void CLKernel::setArg( size_t n, const Vector2f & v )
	{
		_kernel.setArg( ( cl_uint ) n, sizeof( cl_float2 ), ( void* )v.ptr() );
	}
	
	void CLKernel::setArg( size_t n, const Vector3f & v )
	{
		_kernel.setArg( ( cl_uint ) n, sizeof( Vector3f ), ( void* )v.ptr() );
	}
	
	void CLKernel::setArg( size_t n, const Vector4f & v )
	{
		_kernel.setArg( ( cl_uint ) n, sizeof( cl_float4 ), ( void* )v.ptr() );
	}
	
	void CLKernel::setArg( size_t n, const Matrix2f & m )
	{
		_kernel.setArg( ( cl_uint ) n, sizeof( Matrix2f ), ( void* )m.ptr() );
	}
	
	void CLKernel::setArg( size_t n, const Matrix3f & m )
	{
		_kernel.setArg( ( cl_uint ) n, sizeof( Matrix3f ), ( void* )m.ptr() );
	}
	
	void CLKernel::setArg( size_t n, const Matrix4f & m )
	{
		_kernel.setArg( ( cl_uint ) n, sizeof( cl_float16 ), ( void* )m.ptr() );
	}
	
	void CLKernel::setArg( size_t n, const Image & img )
	{
		if( img.memType() != IALLOCATOR_CL )
			throw CVTException("Need Image with CL memory for CLKernel");
		ImageAllocatorCL* clmem = ( ImageAllocatorCL* ) img._mem;
		_kernel.setArg( ( cl_uint ) n, *clmem->_climage );
	}

	void CLKernel::setArg( size_t n, size_t size )
	{
		_kernel.setArg( ( cl_uint ) n, size, NULL );
	}

	void CLKernel::setArg( size_t n, cl::Memory mem )
	{
		_kernel.setArg( ( cl_uint ) n, mem );
	}

	void CLKernel::setArg( size_t n, size_t size, void* ptr )
	{
		_kernel.setArg( ( cl_uint ) n, size, ptr );
	}


	void CLKernel::run( const cl::NDRange& offset, const cl::NDRange& global, const cl::NDRange& local, std::vector<cl::Event>* events, cl::Event* event )
	{
		if( _cl ) {
			cl_int err = _cl->getCLQueue( ).enqueueNDRangeKernel( _kernel, offset, global, local, events, event );
			if( err != CL_SUCCESS ) {
				throw CLException( _name, err );
			}
		}
	}

	void CLKernel::run( const cl::NDRange& global, const cl::NDRange& local, std::vector<cl::Event>* events, cl::Event* event )
	{
		if( _cl ) {
			cl_int err = _cl->getCLQueue( ).enqueueNDRangeKernel( _kernel, cl::NullRange, global, local, events, event );
			if( err != CL_SUCCESS ) {
				throw CLException( _name, err );
			}
		}
	}



	void CLKernel::run( const Image& img, const cl::NDRange& local, std::vector<cl::Event>* events, cl::Event* event )
	{
		if( _cl ) {
			cl_int err = _cl->getCLQueue( ).enqueueNDRangeKernel( _kernel, cl::NullRange, cl::NDRange( img.width(), img.height() ), local, events, event );
			if( err != CL_SUCCESS ) {
				throw CLException( _name, err );
			}
		}
	}


	size_t CLKernel::workGroupSize( ) const
	{
		size_t ret;
		cl_int err;
		err = _kernel.getWorkGroupInfo( _cl->getCLDevice(), CL_KERNEL_WORK_GROUP_SIZE, &ret );
		if( err != CL_SUCCESS )
			throw CLException( _name, err );
		return ret;
	}

	size_t CLKernel::workGroupSizePreferredMultiple( ) const
	{
#ifndef CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE
		return 32;
# else 
		size_t ret;
		cl_int err;
		err = _kernel.getWorkGroupInfo( _cl->getCLDevice(), CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, &ret );
		if( err != CL_SUCCESS )
			throw CLException( _name, err );
		return ret;
#endif
	}

	std::vector<char*>* CLKernel::getBinaries( )
	{
		std::vector<size_t> sizes;
		std::vector<char*>* ret;

		_prog.getInfo( CL_PROGRAM_BINARY_SIZES, &sizes );
		ret = new std::vector<char*>;
		for( std::vector<size_t>::iterator it = sizes.begin(); it != sizes.end(); it++ ) {
			ret->push_back( new char[ *it ] );
		}
		_prog.getInfo( CL_PROGRAM_BINARIES, ret );
		return ret;
	}

}
