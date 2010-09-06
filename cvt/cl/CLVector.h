#ifndef CLVECTOR_H
#define CLVECTOR_H

namespace cvt {
	namespace cl {

		template<typename T, size_t elements>
			class CLVector : public CLParameter
		{
			public:
				CLVector( CLManager* cl );
				const size_t length( ) const
				{
					return elements;
				};
				std::pair<size_t, void*> getCLArgument( )
				{
					return std::make_pair<size_t, void*>( sizeof( T ), &vec );
				};
				cl::NDRange getCLNDRange( ) const
				{
					return cl::NDRange( 1 );
				};
				T get( ) const
				{
					return vec;
				};
				void set( T v )
				{
					vec = v;
				};
				void set( const size_t pos, float a )
				{
					if( pos >= elements ) return;
#if defined( __APPLE__ ) || defined( __MACOSX )
					vec[ pos ] = a;
#else
					vec.s[ pos ] = a;
#endif
				};

				void get( const size_t pos, float& a ) const
				{
					if( pos >= elements ) return;
#if defined( __APPLE__ ) || defined( __MACOSX )
					a = vec[ pos ];
#else
					a = vec.s[ pos ];
#endif
				};

			protected:
				CLVector( const CLVector& ) {};

			private:
				CLVector( );

				CLManager* cl;
				T vec;
		};


#define CLVECTOR_TYPE( a, b, c ) \
		template < > \
		CLVector < a, b > ::CLVector( CLManager * cl ) : CLParameter( c ) \
		{ \
			this->cl = cl; \
		};

		template<>
			void CLVector<cl_float2, 2>::set( float a, float b )
			{
#if defined( __APPLE__ ) || defined( __MACOSX )
				vec[ 0 ] = a;
				vec[ 1 ] = b;
				vec[ 2 ] = c;
				vec[ 3 ] = d;
#else
				vec.s0 = a;
				vec.s1 = b;
				vec.s2 = c;
				vec.s3 = d;
#endif
			}

		template<>
			void CLVector<cl_float2, 2>::get( float& a, float& b ) const
			{
#if defined( __APPLE__ ) || defined( __MACOSX )
				a = vec[ 0 ];
				b = vec[ 1 ];
#else
				a = vec.s0;
				b = vec.s1;
#endif
			}

		template<>
			void CLVector<cl_float4, 4>::set( float a, float b, float c, float d )
			{
#if defined( __APPLE__ ) || defined( __MACOSX )
				vec[ 0 ] = a;
				vec[ 1 ] = b;
				vec[ 2 ] = c;
				vec[ 3 ] = d;
#else
				vec.s0 = a;
				vec.s1 = b;
				vec.s2 = c;
				vec.s3 = d;
#endif
			}

		template<>
			void CLVector<cl_float4, 4>::get( float& a, float& b, float& c, float& d ) const
			{
#if defined( __APPLE__ ) || defined( __MACOSX )
				a = vec[ 0 ];
				b = vec[ 1 ];
				c = vec[ 2 ];
				d = vec[ 3 ];
#else
				a = vec.s0;
				b = vec.s1;
				c = vec.s2;
				d = vec.s3;
#endif
			}

		template<>
			void CLVector<cl_float8, 8>::set( float a, float b, float c, float d, float e, float f, float g, float h )
			{
#if defined( __APPLE__ ) || defined( __MACOSX )
				vec[ 0 ] = a;
				vec[ 1 ] = b;
				vec[ 2 ] = c;
				vec[ 3 ] = d;
				vec[ 4 ] = e;
				vec[ 5 ] = f;
				vec[ 6 ] = g;
				vec[ 7 ] = h;
#else
				vec.s0 = a;
				vec.s1 = b;
				vec.s2 = c;
				vec.s3 = d;
				vec.s4 = e;
				vec.s5 = f;
				vec.s6 = g;
				vec.s7 = h;
#endif
			}

		template<>
			void CLVector<cl_float8, 8>::get( float& a, float& b, float& c, float& d, float& e, float& f, float& g, float& h ) const
			{
#if defined( __APPLE__ ) || defined( __MACOSX )
				a = vec[ 0 ];
				b = vec[ 1 ];
				c = vec[ 2 ];
				d = vec[ 3 ];
				e = vec[ 4 ];
				f = vec[ 5 ];
				g = vec[ 6 ];
				h = vec[ 7 ];
#else
				a = vec.s0;
				b = vec.s1;
				c = vec.s2;
				d = vec.s3;
				e = vec.s4;
				f = vec.s5;
				g = vec.s6;
				h = vec.s7;
#endif
			}

		CLVECTOR_TYPE( cl_float2, 2, CL_PARAM_VECTOR2 )
			CLVECTOR_TYPE( cl_float4, 4, CL_PARAM_VECTOR4 )
			CLVECTOR_TYPE( cl_float4, 8, CL_PARAM_VECTOR8 )
			CLVECTOR_TYPE( cl_float4, 16, CL_PARAM_VECTOR16 )

			typedef CLVector<cl_float2, 2> CLVector2;
		typedef CLVector<cl_float4, 4> CLVector4;
		typedef CLVector<cl_float8, 8> CLVector8;
		typedef CLVector<cl_float16, 16> CLVector16;


	}
}
#endif
