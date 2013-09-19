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
																											  -R^T t | 1
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

inline void planef_homography( Mat3f* h, const Planef p, const Mat4f* Kdst, const Mat4f* Tdst, const Mat4f* Ksrc, const Mat4f* Tsrc )
{

}

inline float planef_intersect_ray( Planef p, float3 ray )
{
	// FIXME: + or - p.w
	return - p.w / dot( p.xyz, ray );
}
