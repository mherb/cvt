#include <cvt/cl/CLColor.h>

namespace cvt {
	namespace cl {

		CLColor::CLColor( CLManager* cl ) : CLParameter( CL_PARAM_COLOR )
		{
			this->cl = cl;
			set( 1.0f, 1.0f, 1.0f, 1.0f );
		}

		CLColor::~CLColor( )
		{}

		void CLColor::set( float r, float g, float b, float a )
		{
#if defined( __APPLE__ ) || defined( __MACOSX )
			color[ 0 ] = r;
			color[ 1 ] = g;
			color[ 2 ] = b;
			color[ 3 ] = a;
#else
			color.s[ 0 ] = r;
			color.s[ 1 ] = g;
			color.s[ 2 ] = b;
			color.s[ 3 ] = a;
#endif
		}

		void CLColor::get( float& r, float& g, float& b, float& a ) const
		{
#if defined( __APPLE__ ) || defined( __MACOSX )
			r = color[ 0 ];
			g = color[ 1 ];
			b = color[ 2 ];
			a = color[ 3 ];
#else
			r = color.s[ 0 ];
			g = color.s[ 1 ];
			b = color.s[ 2 ];
			a = color.s[ 3 ];
#endif
		}

		std::pair<size_t, void*> CLColor::getCLArgument( )
		{
			return std::make_pair<size_t, void*>( sizeof( cl_float4 ), &color );
		}

		::cl::NDRange CLColor::getCLNDRange( ) const
		{
			return ::cl::NDRange( 1 );
		}

	}
}
