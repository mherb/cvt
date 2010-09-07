#ifndef IFILTERVECTOR_H
#define IFILTERVECTOR_H
#include <stdlib.h>
#include <cvt/gfx/IFilterParameter.h>

namespace cvt {

	template<typename T, size_t elements>
	class IFilterVector : public IFilterParameter
	{
		friend class CLKernel;

		public:
			IFilterVector();
			IFilterVector( const IFilterVector& v );

			size_t length( ) const
			{
				return elements;
			};

			void set( const size_t pos, float a )
			{
				if( pos >= elements ) return;
				vec[ pos ] = a;
			};

			void get( const size_t pos, float& a ) const
			{
				if( pos >= elements ) return;
				a = vec[ pos ];
			};

			void set( float, float );
			void get( float&, float& ) const;
			void set( float, float, float, float );
			void get( float&, float&, float&, float& ) const;
			void set( float, float, float, float, float, float, float, float );
			void get( float&, float&, float&, float&, float&, float&, float&, float& ) const;

		protected:
			T vec[ elements ] __attribute__((aligned(16)));
	};


#define IFILTERVECTOR_TYPE( a, b, c ) \
	template<> \
	IFilterVector<a,b>::IFilterVector() : IFilterParameter( c ) {}; \
	template<> \
	IFilterVector<a,b>::IFilterVector( const IFilterVector& ifv ) : IFilterParameter( c ) \
	{ \
		for( size_t i = 0; i < b; i++ ) \
			vec[ i ] = ifv.vec[ i ]; \
	}; \


	template<>
	void IFilterVector<float,2>::set( float a, float b )
	{
		vec[ 0 ] = a;
		vec[ 1 ] = b;
	}

	template<>
	void IFilterVector<float,2>::get( float& a, float& b ) const
	{
		a = vec[ 0 ];
		b = vec[ 1 ];
	}

	template<>
		void IFilterVector<float, 4>::set( float a, float b, float c, float d )
		{
			vec[ 0 ] = a;
			vec[ 1 ] = b;
			vec[ 2 ] = c;
			vec[ 3 ] = d;
		}

	template<>
		void IFilterVector<float, 4>::get( float& a, float& b, float& c, float& d ) const
		{
			a = vec[ 0 ];
			b = vec[ 1 ];
			c = vec[ 2 ];
			d = vec[ 3 ];
		}

	template<>
		void IFilterVector<float, 8>::set( float a, float b, float c, float d, float e, float f, float g, float h )
		{
			vec[ 0 ] = a;
			vec[ 1 ] = b;
			vec[ 2 ] = c;
			vec[ 3 ] = d;
			vec[ 4 ] = e;
			vec[ 5 ] = f;
			vec[ 6 ] = g;
			vec[ 7 ] = h;
		}

	template<>
		void IFilterVector<float, 8>::get( float& a, float& b, float& c, float& d, float& e, float& f, float& g, float& h ) const
		{
			a = vec[ 0 ];
			b = vec[ 1 ];
			c = vec[ 2 ];
			d = vec[ 3 ];
			e = vec[ 4 ];
			f = vec[ 5 ];
			g = vec[ 6 ];
			h = vec[ 7 ];
		}

	IFILTERVECTOR_TYPE( float, 2, IFILTERPARAMETER_VECTOR2 )
	IFILTERVECTOR_TYPE( float, 4, IFILTERPARAMETER_VECTOR4 )
	IFILTERVECTOR_TYPE( float, 8, IFILTERPARAMETER_VECTOR8 )
	IFILTERVECTOR_TYPE( float, 16, IFILTERPARAMETER_VECTOR16 )

	typedef IFilterVector<float, 2> IFilterVector2;
	typedef IFilterVector<float, 4> IFilterVector4;
	typedef IFilterVector<float, 8> IFilterVector8;
	typedef IFilterVector<float, 16> IFilterVector16;
}

#endif
