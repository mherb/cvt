#include <cvt/cl/CLFilter.h>
#include <cvt/util/Time.h>

namespace cvt {

		CLFilter::CLFilter( CLManager* cl, std::string n, const CLParameterInfo* ps, size_t psize ) : name( n ), pinfo( ps ), pinfosize( psize )
		{
			this->cl = cl;
		}

		std::ostream& operator <<( std::ostream &out, const CLFilter &f )
		{
			return f.print( out );
		}

		std::ostream& CLFilter::print( std::ostream& out ) const
		{
			size_t i;

			out << "Filter:" << name << " ( " << pinfosize << " Parameter(s) ) :" << std::endl;
			for( i = 0; i < pinfosize; i++ ) {
				std::string pname;
				switch( pinfo[ i ].getType( ) ) {
					case CL_PARAM_IMAGE: pname = "CL_IMAGE";
										 break;
					case CL_PARAM_SCALAR: pname = "CL_SCALAR";
										  break;
					case CL_PARAM_COLOR: pname = "CL_COLOR";
										 break;
					default: pname = "UNKOWN";
							 break;
				}
				out << "\tParameter " << i << ": \"" << pinfo[ i ].getName( ) << "\" " << ( pinfo[ i ].getDirection( ) == CL_PARAM_IN ? "INPUT " : "OUTPUT " ) << pname << std::endl;
			}
			return out;
		}

		void CLFilter::buildKernel( ::cl::Kernel& kernel, const char* name, const char* src, size_t len, std::string& buildinfo )
		{
			cl_int err;
			::cl::Program::Sources source( 1, std::make_pair<const char*, size_t>( src, len ) );
			std::vector< ::cl::Device> devices;

			::cl::Program prog( cl->getCLContext( ), source, &err );
			if( err != CL_SUCCESS ) {
				throw CLException( err );
			}

			devices.push_back( cl->getCLDevice( ) );
			err = prog.build( devices, "-w" );
			buildinfo = prog.getBuildInfo<CL_PROGRAM_BUILD_LOG>( cl->getCLDevice( ) );
			if( err != CL_SUCCESS ) {
				throw CLException( err, buildinfo );
			}

			kernel = ::cl::Kernel( prog, name, &err );
			if( err != CL_SUCCESS ) {
				throw CLException( err );
			}
		}

		void CLFilter::runKernel( const ::cl::Kernel& kernel, const ::cl::NDRange& offset, const ::cl::NDRange& global, const ::cl::NDRange& local, std::vector< ::cl::Event>* events, ::cl::Event* event )
		{
			if( cl ) {
				cl_int err = cl->getCLQueue( ).enqueueNDRangeKernel( kernel, offset, global, local, events, event );
				if( err != CL_SUCCESS ) {
					throw CLException( err );
				}
			}
		}

		CLParameterSet* CLFilter::getParameterSet( void )
		{
			return new CLParameterSet( pinfo, pinfosize );
		}

		void CLFilter::apply( CLParameterSet* set, double& t )
		{
			Time timer;

			apply( set );
			cl->getCLQueue( ).finish( );
			t = timer.elapsedSeconds( );
		}

}
