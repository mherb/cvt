#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLImage.h>
#include <cvt/gfx/Color.h>
#include <cvt/gfx/IFilterScalar.h>
#include <cvt/gfx/IFilterVector.h>

namespace cvt {

	void CLKernel::build( const char* name, const char* src, size_t len, std::string& buildinfo )
	{
		cl_int err;
		cl::Program::Sources source( 1, std::make_pair<const char*, size_t>( src, len ) );
		std::vector<cl::Device> devices;

		_cl = CLContext::getCurrent();

		cl::Program prog( _cl->getCLContext( ), source, &err );
		if( err != CL_SUCCESS ) {
			throw CLException( err );
		}

		devices.push_back( _cl->getCLDevice( ) );
		err = prog.build( devices, "-w" );
		buildinfo = prog.getBuildInfo<CL_PROGRAM_BUILD_LOG>( _cl->getCLDevice( ) );
		if( err != CL_SUCCESS ) {
			throw CLException( err, buildinfo );
		}

		_kernel = cl::Kernel( prog, name, &err );
		if( err != CL_SUCCESS ) {
			throw CLException( err );
		}
	}

	void CLKernel::setArg( size_t i, IFilterParameter* p )
	{
		switch( p->getIFilterParameterType() )
		{
			case IFILTERPARAMETER_COLOR:
				_kernel.setArg( ( cl_uint ) i, sizeof( cl_float4 ), ( void* ) ( ( Color* ) p )->v );
				break;
			case IFILTERPARAMETER_SCALAR:
				_kernel.setArg( ( cl_uint ) i, sizeof( cl_float ), ( void* ) &( ( IFilterScalar* ) p )->value );
				break;
			case IFILTERPARAMETER_VECTOR2:
				_kernel.setArg( ( cl_uint ) i, sizeof( cl_float2 ), ( void* ) ( ( IFilterVector2* ) p )->vec );
				break;
			case IFILTERPARAMETER_VECTOR4:
				_kernel.setArg( ( cl_uint ) i, sizeof( cl_float4 ), ( void* ) ( ( IFilterVector4* ) p )->vec );
				break;
			case IFILTERPARAMETER_VECTOR8:
				_kernel.setArg( ( cl_uint ) i, sizeof( cl_float8 ), ( void* ) ( ( IFilterVector8* ) p )->vec );
				break;
			case IFILTERPARAMETER_VECTOR16:
				_kernel.setArg( ( cl_uint ) i, sizeof( cl_float16 ), ( void* ) ( ( IFilterVector16* ) p )->vec );
				break;
			case IFILTERPARAMETER_IMAGE:
				CLImage* img;
				if( ( img = dynamic_cast<CLImage*>( p ) ) != NULL ) {
					_kernel.setArg( ( cl_uint ) i, img->_climage() );
				} else
					throw CVTException("Need CLImage for CLKernel");
				break;
		}
	}

	void CLKernel::run( const cl::NDRange& offset, const cl::NDRange& global, const cl::NDRange& local, std::vector<cl::Event>* events, cl::Event* event )
	{
		if( _cl ) {
			cl_int err = _cl->getCLQueue( ).enqueueNDRangeKernel( _kernel, offset, global, local, events, event );
			if( err != CL_SUCCESS ) {
				throw CLException( err );
			}
		}
	}

}
