#import "Matrix3.cl"
#import "Matrix4.cl"

typedef float4 Planef

inline Planef planef_init( float3 normal, float depth )
{
	float4 p;
	p.xyz = normal;
	p.w   = depth; // FIXME: use -depth
	return p;
}

inline Planef planef_transform( Planef p, const Mat4f* mat )
{
	Planef ret;
	// transform using mat^-T
	// assume mat = [ R | t ] format -> mat^-T = ( mat^-1 )^ T = ( [ R | t ]^-1 )^T = ( [ R^T | -R^T t] )^T =  R     | 0
	//																										  -R^T t | 1
	Mat3f RT;

	// ( R^-1 )^T = ( R^T )^T = R
	RT.m[ 0 ].xyz = ( float3 ) ( mat->m[ 0 ].xyz);
	RT.m[ 1 ].xyz = ( float3 ) ( mat->m[ 1 ].xyz );
	RT.m[ 2 ].xyz = ( float3 ) ( mat->m[ 2 ].xyz );

	return ( float3 ) ( mat3f_transform( &RT, p.xyz ),
					    dot( ( float4 ) ( -mat3f_transpose_transform( &RT, ( float3 )( mat->m[ 0 ].w, mat->m[ 1 ].w, mat->m[ 2 ].w ), 1.0f ), p ) );
}

inline float planef_distance( Planef p, float3 pt )
{
	return dot( p, ( float4 ) ( pt, 1.0f ) );
}

inline void planef_homography( Mat3f* H, const Planef p, const Mat3f* Kdst, const Mat4f* Tdst, const Mat3f* KsrcInverse, const Mat4f* Tsrc )
{
	Mat3f RR, m3tmp, m3tmp2;

	m3tmp.m[ 0 ]  = Tdst->m[ 0 ].xyz;
	m3tmp.m[ 1 ]  = Tdst->m[ 1 ].xyz;
	m3tmp.m[ 2 ]  = Tdst->m[ 2 ].xyz;

	m3tmp2.m[ 0 ] = Tsrc->m[ 0 ].xyz;
	m3tmp2.m[ 1 ] = Tsrc->m[ 1 ].xyz;
	m3tmp2.m[ 2 ] = Tsrc->m[ 2 ].xyz;

	// Rdst * Rsrc^T
	mat3f_mul_transpose( &RR, &m3tmp, &m3tmp2 );

	// get translation parts
	float3 tdst = ( float3 ) ( Tdst->m[ 0 ].w, Tdst->m[ 1 ].w, Tdst->m[ 2 ].w );
	float3 tsrc = ( float3 ) ( Tsrc->m[ 0 ].w, Tsrc->m[ 1 ].w, Tsrc->m[ 2 ].w );

	// Outer = ( 1 / depth ) * ( tdst - Rdst * Rsrc^T tsrc ) * normal
	mat3f_outer( &m3tmp, ( tdst - mat3f_mul( &RR, tsrc ) ) / p.w, p.xyz );

	// Kdst *(  Rdst * Rsrc^T - Outer ) * KsrcInverse
	mat3f_sub( &m3tmp2, &RR, &m3tmp );
	mat3f_mul( &m3tmp, Kdst, &m3tmp2 );
	mat3f_mul( H, &m3tmp, KsrcInverse );
}

inline float planef_intersect_ray( Planef p, float3 ray )
{
	// FIXME: + or - p.w
	return - p.w / dot( p.xyz, ray );
}
