#ifndef CLVECTOR_H
#define CLVECTOR_H

template<typename T, size_t N>
class IFilterVector : public IFilterParameter
{
	public:

		const size_t length( ) const
		{
			return N;
		};

		void set( const size_t pos, float a )
		{
			if( pos >= N ) return;
			vec[ pos ] = a;
		};

		void get( const size_t pos, float& a ) const
		{
			if( pos >= N ) return;
			a = vec[ pos ];
		};

	protected:
		T vec[ N ];
};


#define IFILTERVECTOR_TYPE( a, b, c ) \
	template < > \
	IFilterVector < a, b > ::IFilterVector( ) : IFilterParameter( c ) {}; \
	IFilterVector( const IFilterVector& ifv ) : IFilterParameter( c ) \
		{ \
			for( size_t i = 0; i < N; i++ ) \
				vec[ i ] = ifv.vec[ i ]; \
		}; \

template<>
void IFilterVector<float, 2>::set( float a, float b )
{
	vec[ 0 ] = a;
	vec[ 1 ] = b;
}

template<>
void IFilterVector<float, 2>::get( float& a, float& b ) const
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

IFILTERVECTOR_TYPE( float, 2, CL_PARAM_VECTOR2 )
IFILTERVECTOR_TYPE( float, 4, CL_PARAM_VECTOR4 )
IFILTERVECTOR_TYPE( float, 8, CL_PARAM_VECTOR8 )
IFILTERVECTOR_TYPE( float, 16, CL_PARAM_VECTOR16 )

typedef IFilterVector<float, 2> IFilterVector2;
typedef IFilterVector<float, 4> IFilterVector4;
typedef IFilterVector<float, 8> IFilterVector8;
typedef IFilterVector<float, 16> IFilterVector16;


#endif
