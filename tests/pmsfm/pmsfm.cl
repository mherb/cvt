#pragma SELECT_ROUNDING_MODE rtz

/*
Part of MWC64X by David Thomas, dt10@imperial.ac.uk
This is provided under BSD, full license is with the main package.
See http://www.doc.ic.ac.uk/~dt10/research
*/
#ifndef dt10_mwc64x_rng_cl
#define dt10_mwc64x_rng_cl

// Pre: a<M, b<M
// Post: r=(a+b) mod M
ulong MWC_AddMod64(ulong a, ulong b, ulong M)
{
	ulong v=a+b;
	if( (v>=M) || (v<a) )
		v=v-M;
	return v;
}

// Pre: a<M,b<M
// Post: r=(a*b) mod M
// This could be done more efficently, but it is portable, and should
// be easy to understand. It can be replaced with any of the better
// modular multiplication algorithms (for example if you know you have
// double precision available or something).
ulong MWC_MulMod64(ulong a, ulong b, ulong M)
{	
	ulong r=0;
	while(a!=0){
		if(a&1)
			r=MWC_AddMod64(r,b,M);
		b=MWC_AddMod64(b,b,M);
		a=a>>1;
	}
	return r;
}


// Pre: a<M, e>=0
// Post: r=(a^b) mod M
// This takes at most ~64^2 modular additions, so probably about 2^15 or so instructions on
// most architectures
ulong MWC_PowMod64(ulong a, ulong e, ulong M)
{
	ulong sqr=a, acc=1;
	while(e!=0){
		if(e&1)
			acc=MWC_MulMod64(acc,sqr,M);
		sqr=MWC_MulMod64(sqr,sqr,M);
		e=e>>1;
	}
	return acc;
}

uint2 MWC_SkipImpl_Mod64(uint2 curr, ulong A, ulong M, ulong distance)
{
	ulong m=MWC_PowMod64(A, distance, M);
	ulong x=curr.x*(ulong)A+curr.y;
	x=MWC_MulMod64(x, m, M);
	return (uint2)((uint)(x/A), (uint)(x%A));
}

uint2 MWC_SeedImpl_Mod64(ulong A, ulong M, uint vecSize, uint vecOffset, ulong streamBase, ulong streamGap)
{
	// This is an arbitrary constant for starting LCG jumping from. I didn't
	// want to start from 1, as then you end up with the two or three first values
	// being a bit poor in ones - once you've decided that, one constant is as
	// good as any another. There is no deep mathematical reason for it, I just
	// generated a random number.
	enum{ MWC_BASEID = 4077358422479273989UL };
	
	ulong dist=streamBase + (get_global_id(0)*vecSize+vecOffset)*streamGap;
	ulong m=MWC_PowMod64(A, dist, M);
	
	ulong x=MWC_MulMod64(MWC_BASEID, m, M);
	return (uint2)((uint)(x/A), (uint)(x%A));
}

//! Represents the state of a particular generator
typedef struct{ uint x; uint c; } mwc64x_state_t;

enum{ MWC64X_A = 4294883355U };
enum{ MWC64X_M = 18446383549859758079UL };

void MWC64X_Step(mwc64x_state_t *s)
{
	uint X=s->x, C=s->c;
	
	uint Xn=MWC64X_A*X+C;
	uint carry=(uint)(Xn<C);				// The (Xn<C) will be zero or one for scalar
	uint Cn=mad_hi(MWC64X_A,X,carry);  
	
	s->x=Xn;
	s->c=Cn;
}

void MWC64X_Skip(mwc64x_state_t *s, ulong distance)
{
	uint2 tmp=MWC_SkipImpl_Mod64((uint2)(s->x,s->c), MWC64X_A, MWC64X_M, distance);
	s->x=tmp.x;
	s->c=tmp.y;
}

void MWC64X_SeedStreams(mwc64x_state_t *s, ulong baseOffset, ulong perStreamOffset)
{
	uint2 tmp=MWC_SeedImpl_Mod64(MWC64X_A, MWC64X_M, 1, 0, baseOffset, perStreamOffset);
	s->x=tmp.x;
	s->c=tmp.y;
}

//! Return a 32-bit integer in the range [0..2^32)
uint MWC64X_NextUint(mwc64x_state_t *s)
{
	uint res=s->x ^ s->c;
	MWC64X_Step(s);
	return res;
}

float MWC64X_NextFloat(mwc64x_state_t *s)
{
	uint res=s->x ^ s->c;
	MWC64X_Step(s);
	return  2.3283064365386962890625e-10f * ( float ) res;
}
#endif

typedef mwc64x_state_t RNG;

#define RNG_init( x, linpos, numdraws ) do {\
	MWC64X_SeedStreams( &rng, 0, 0 ); \
	MWC64X_Skip( &rng, ( linpos ) * numdraws ); \
} while( 0 )

#define RNG_float( x ) MWC64X_NextFloat( x )

// values in meter
#define DEPTHMAX  0.2f
#define DEPTHMIN  1e-6f

#define TRANSMAX  1.0f
#define ROTMAX	  0.225f;
#define NORMALCOMPMAX 0.95f

#define PROPSIZE 1
#define NUMRNDTRIES	 2
#define NUMRNDSAMPLE  2

#define DEPTHREFINEMUL 0.001f
#define NORMALREFINEMUL 0.05f
#define TRANSREFINEMUL  0.05f
#define ROTREFINEMUL ( ( 0.25f / 180.0f ) * M_PI )

#define COLORWEIGHT 16.0f
#define COLORGRADALPHA 0.1f
#define COLORMAXDIFF 0.2f
#define GRADMAXDIFF 0.1f
#define OVERSAMPLE 1.0f
#define OVERSAMPLECUBE 0.01f

// #define USELOCALBUF  1

const sampler_t SAMPLER_NN		 = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
const sampler_t SAMPLER_BILINEAR = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

struct Mat3 {
	float3 m[ 3 ];
};

typedef struct Mat3 Mat3;

inline float3 Mat3_MulVec( Mat3* mat, const float3 vec )
{
	return ( float3 ) ( dot( mat->m[ 0 ].xyz, vec ),
					   dot( mat->m[ 1 ].xyz, vec ),
					   dot( mat->m[ 2 ].xyz, vec ) );
}

inline float3 Mat3_TransMulVec( Mat3* mat, const float3 vec )
{
	return ( float3 ) ( dot( ( float3 ) ( mat->m[ 0 ].x, mat->m[ 1 ].x, mat->m[ 2 ].x ), vec ),
						dot( ( float3 ) ( mat->m[ 0 ].y, mat->m[ 1 ].y, mat->m[ 2 ].y ), vec ),
						dot( ( float3 ) ( mat->m[ 0 ].z, mat->m[ 1 ].z, mat->m[ 2 ].z ), vec ) );
}



inline float2 Mat3_MulVecProj2( Mat3* mat, const float2 vec )
{
	float3 hvec = ( float3 ) ( vec.x, vec.y, 1.0f );
	float3 tmp = ( float3 ) ( dot( mat->m[ 0 ].xyz, hvec ),
						      dot( mat->m[ 1 ].xyz, hvec ),
				              dot( mat->m[ 2 ].xyz, hvec ) );
	return ( float2 ) ( tmp.x / tmp.z, tmp.y / tmp.z );
}

inline void Mat3_Outer( Mat3* dst, const float3 a, const float3 b )
{
	dst->m[ 0 ].xyz = a.x * b;
	dst->m[ 1 ].xyz = a.y * b;
	dst->m[ 2 ].xyz = a.z * b;
}

inline void Mat3_Mul( Mat3* dst, const Mat3* a, const Mat3* b )
{
	dst->m[ 0 ].x = dot( a->m[ 0 ].xyz, ( float3 ) ( b->m[ 0 ].x, b->m[ 1 ].x, b->m[ 2 ].x ) );
	dst->m[ 0 ].y = dot( a->m[ 0 ].xyz, ( float3 ) ( b->m[ 0 ].y, b->m[ 1 ].y, b->m[ 2 ].y ) );
	dst->m[ 0 ].z = dot( a->m[ 0 ].xyz, ( float3 ) ( b->m[ 0 ].z, b->m[ 1 ].z, b->m[ 2 ].z ) );

	dst->m[ 1 ].x = dot( a->m[ 1 ].xyz, ( float3 ) ( b->m[ 0 ].x, b->m[ 1 ].x, b->m[ 2 ].x ) );
	dst->m[ 1 ].y = dot( a->m[ 1 ].xyz, ( float3 ) ( b->m[ 0 ].y, b->m[ 1 ].y, b->m[ 2 ].y ) );
	dst->m[ 1 ].z = dot( a->m[ 1 ].xyz, ( float3 ) ( b->m[ 0 ].z, b->m[ 1 ].z, b->m[ 2 ].z ) );

	dst->m[ 2 ].x = dot( a->m[ 2 ].xyz, ( float3 ) ( b->m[ 0 ].x, b->m[ 1 ].x, b->m[ 2 ].x ) );
	dst->m[ 2 ].y = dot( a->m[ 2 ].xyz, ( float3 ) ( b->m[ 0 ].y, b->m[ 1 ].y, b->m[ 2 ].y ) );
	dst->m[ 2 ].z = dot( a->m[ 2 ].xyz, ( float3 ) ( b->m[ 0 ].z, b->m[ 1 ].z, b->m[ 2 ].z ) );
}


inline void Mat3_MulTranspose( Mat3* dst, const Mat3* a, const Mat3* b )
{
	dst->m[ 0 ].x = dot( a->m[ 0 ].xyz, b->m[ 0 ].xyz );
	dst->m[ 0 ].y = dot( a->m[ 0 ].xyz, b->m[ 1 ].xyz );
	dst->m[ 0 ].z = dot( a->m[ 0 ].xyz, b->m[ 2 ].xyz );

	dst->m[ 1 ].x = dot( a->m[ 1 ].xyz, b->m[ 0 ].xyz );
	dst->m[ 1 ].y = dot( a->m[ 1 ].xyz, b->m[ 1 ].xyz );
	dst->m[ 1 ].z = dot( a->m[ 1 ].xyz, b->m[ 2 ].xyz );

	dst->m[ 2 ].x = dot( a->m[ 2 ].xyz, b->m[ 0 ].xyz );
	dst->m[ 2 ].y = dot( a->m[ 2 ].xyz, b->m[ 1 ].xyz );
	dst->m[ 2 ].z = dot( a->m[ 2 ].xyz, b->m[ 2 ].xyz );
}

inline void Mat3_Add( Mat3* dst, Mat3* a, Mat3* b )
{
	dst->m[ 0 ] = a->m[ 0 ] + b->m[ 0 ];
	dst->m[ 1 ] = a->m[ 1 ] + b->m[ 1 ];
	dst->m[ 2 ] = a->m[ 2 ] + b->m[ 2 ];
}

inline void Mat3_Sub( Mat3* dst, Mat3* a, Mat3* b )
{
	dst->m[ 0 ] = a->m[ 0 ] - b->m[ 0 ];
	dst->m[ 1 ] = a->m[ 1 ] - b->m[ 1 ];
	dst->m[ 2 ] = a->m[ 2 ] - b->m[ 2 ];
}

inline void Mat3_AxisAngleRotation( Mat3* mat, float3 rot )
{
        float rad = length( rot );
		float3 axis = normalize( rot );
        float x, y, z, c, s;
        float wx, wy, wz;
        float xx, yy, yz;
        float xy, xz, zz;
        float x2, y2, z2;

        c = cos( rad * 0.5f );
        s = sin( rad * 0.5f );

        x = axis.x * s;
        y = axis.y * s;
        z = axis.z * s;

        x2 = x + x;
        y2 = y + y;
        z2 = z + z;

        xx = x * x2;
        xy = x * y2;
        xz = x * z2;

        yy = y * y2;
        yz = y * z2;
        zz = z * z2;

        wx = c * x2;
        wy = c * y2;
        wz = c * z2;

        mat->m[ 0 ].x = 1.0f - ( yy + zz );
        mat->m[ 0 ].y = xy - wz;
        mat->m[ 0 ].z = xz + wy;

        mat->m[ 1 ].x = xy + wz;
        mat->m[ 1 ].y = 1.0f - ( xx + zz );
        mat->m[ 1 ].z = yz - wx;

        mat->m[ 2 ].x = xz - wy;
        mat->m[ 2 ].y = yz + wx;
        mat->m[ 2 ].z = 1.0f - ( xx + yy );
}

inline void Mat3_Rotation( Mat3* mat, float3 rot )
{
	float3 c;
	float3 s = sincos( rot, &c );

	mat->m[ 0 ].x =  c.y * c.z;
	mat->m[ 0 ].y = -c.y * s.z;
	mat->m[ 0 ].z =        s.y;

	mat->m[ 1 ].x = c.x * s.z + c.z * s.x * s.y;
	mat->m[ 1 ].y = c.x * c.z - s.x *  s.y * s.z;
	mat->m[ 1 ].z =               -c.y * s.x;

	mat->m[ 2 ].x =  s.x * s.z - c.x * c.z *  s.y;
	mat->m[ 2 ].y =  c.x * s.y * s.z + c.z * s.x;
	mat->m[ 2 ].z =                    c.x * c.y;
}

inline void Mat3_RotationTranspose( Mat3* mat, float3 rot )
{
	float3 c;
	float3 s = sincos( rot, &c );

	mat->m[ 0 ].x =  c.y * c.z;
	mat->m[ 1 ].x = -c.y * s.z;
	mat->m[ 2 ].x =        s.y;

	mat->m[ 0 ].y = c.x * s.z + c.z * s.x *  s.y;
	mat->m[ 1 ].y = c.x * c.z - s.x *  s.y * s.z;
	mat->m[ 2 ].y =               -c.y * s.x;

	mat->m[ 0 ].z =       s.x * s.z - c.x * c.z *  s.y;
	mat->m[ 1 ].z =  c.x * s.y * s.z +     c.z * s.x;
	mat->m[ 2 ].z =                         c.x * c.y;
}

float8 pmsfm_state_init( RNG* rng )
{
	float8 ret;
	float z;
	float3 n;
	float3 rot;
	float3 trans;

	z = DEPTHMIN + RNG_float( rng ) * ( DEPTHMAX - DEPTHMIN  ) ;

	n.x = ( RNG_float( rng ) - 0.5f ) * 0.5f * NORMALCOMPMAX;
	n.y = ( RNG_float( rng ) - 0.5f ) * 0.5f * NORMALCOMPMAX;

	rot.x = ( RNG_float( rng ) - 0.5f ) * 2.0f * ROTMAX;
	rot.z = ( RNG_float( rng ) - 0.5f ) * 2.0f * ROTMAX;
	rot.y = sqrt( 1.0f - rot.x * rot.x - rot.z * rot.z );
	rot  *= ( RNG_float( rng ) - 0.5f ) * 2.0f * ( 15.0f / 180.0f ) * M_PI;

	trans.x = ( RNG_float( rng ) - 0.5f ) * 2.0f * TRANSMAX;
	trans.y = ( RNG_float( rng ) - 0.5f ) * 2.0f * TRANSMAX;
	trans.z = ( RNG_float( rng ) - 0.5f ) * 2.0f * TRANSMAX;
	trans = normalize( trans );

	trans *= ( float3 ) z;

	return ( float8 )( rot.x, rot.y, rot.z, trans.x, trans.y, trans.z, n.x, n.y );
}

float8 pmsfm_state_refine( RNG* rng, const float8 state )
{
	float8 ret;
	float z, len, rotlen;
	float3 n;
	float3 rot;
	float3 trans;

	z = len = length( state.s345 );
	z += ( RNG_float( rng ) - 0.5f ) * 2.0f * DEPTHREFINEMUL;
	z = clamp( z, DEPTHMIN, DEPTHMAX );

	n.xy = state.s67;
	n.x += ( RNG_float( rng ) - 0.5f ) * 2.0f * NORMALREFINEMUL;
	n.y += ( RNG_float( rng ) - 0.5f ) * 2.0f * NORMALREFINEMUL;
	n.x = clamp( n.x, -NORMALCOMPMAX, NORMALCOMPMAX );
	n.y = clamp( n.y, -NORMALCOMPMAX, NORMALCOMPMAX );

	rot   = state.s012;
	rotlen = length( rot );
	rot   = normalize( rot );
	rot.x += ( RNG_float( rng ) - 0.5f ) * 2.0f * ROTREFINEMUL;
	rot.z += ( RNG_float( rng ) - 0.5f ) * 2.0f * ROTREFINEMUL;
	rot.x = clamp( rot.x, -ROTMAX, ROTMAX );
	rot.y = clamp( rot.y, -ROTMAX, ROTMAX );
	rot.y = sqrt( 1.0f - rot.x * rot.x - rot.z * rot.z );
	rotlen += ( RNG_float( rng ) - 0.5f ) * 2.0f * ( 1.0f / 180.0f ) * M_PI;
	rotlen = clamp( rotlen, -( 15.0f / 180.0f ) * M_PI, ( 15.0f / 180.0f ) * M_PI );
	rot  *= rotlen;

	trans = normalize( state.s345 );
	trans.x += ( RNG_float( rng ) - 0.5f ) * 2.0f * ( TRANSREFINEMUL );
	trans.y += ( RNG_float( rng ) - 0.5f ) * 2.0f * ( TRANSREFINEMUL );
	trans.z += ( RNG_float( rng ) - 0.5f ) * 2.0f * ( TRANSREFINEMUL );
//	trans.x = clamp( trans.x, -TRANSMAX/DEPTHMAX, TRANSMAX/DEPTHMAX );
//	trans.y = clamp( trans.y, -TRANSMAX/DEPTHMAX, TRANSMAX/DEPTHMAX );
//	trans.z = clamp( trans.z, -TRANSMAX/DEPTHMAX, TRANSMAX/DEPTHMAX );
	trans = normalize( trans );
	trans *= z;

	return ( float8 )( rot.x, rot.y, rot.z, trans.x, trans.y, trans.z, n.x, n.y );
}

inline void pmsfm_state_to_matrix( const float8 state, Mat3* matrix, const Mat3* Kdst, const Mat3* Ksrc )
{
	Mat3 rot, outer, tmp;
	// rotation matrix
	Mat3_AxisAngleRotation( &rot, state.s012 );
	// translation
	float3 t = state.s345;
	// normal
	float3 n = ( float3 ) ( state.s6, state.s7, sqrt( 1.0f - state.s6 * state.s6 - state.s7 * state.s7 ) );
	// K_dst ( R^T + (1/d) (R^T t) n^T ) K_src
	Mat3_Outer( &outer, /*Mat3_TransMulVec( &rot, t )*/ -t, n );
	Mat3_Add( &tmp, &rot, &outer );
	Mat3_Mul( &rot,  &tmp, Ksrc );
	Mat3_Mul( matrix, Kdst, &rot );
}

float8 pmsfm_state_viewprop( const float8 state )
{
	// TODO, if needed at all
	return ( float8 ) 0.0f;
}

float3 pmsfm_state_to_normal( const float8 state )
{
	return ( float3 ) ( state.s6, state.s7, sqrt( 1.0f - state.s6 * state.s6 - state.s7 * state.s7 ) );
}

float pmsfm_state_to_depth( const float8 state )
{
	return length( state.s345 );
}

float pmsfm_state_to_unitdepth( const float8 state )
{
	return length( state.s345 ) / ( DEPTHMAX );
}


inline float patch_eval_color_grad_weighted( read_only image2d_t colimg1, read_only image2d_t gradimg1,
											 read_only image2d_t colimg2, read_only image2d_t gradimg2,
											 const float2 coord, const float8 state, const Mat3* Kdst, const Mat3* Ksrc, const int patchsize )
{
	float wsum1 = 0;
	float ret1 = 0;
	int width = get_image_width( colimg2 );
	int height = get_image_height( colimg2 );
	Mat3 mat;
	pmsfm_state_to_matrix( state, &mat, Kdst, Ksrc );

//	const float4 grayWeight =  ( float4 ) ( 0.2126f, 0.7152f, 0.0722f, 0.0f );

	float4 valcenter = read_imagef( colimg1, SAMPLER_BILINEAR, coord + ( float2 ) ( 0.5f, 0.5f) );

	for( float dy = -patchsize; dy <= patchsize; dy+=1.0f ) {
		for( float dx = -patchsize; dx <= patchsize; dx+=1.0f ) {

			float2 displace = ( float2 ) ( dx * OVERSAMPLE, dy * OVERSAMPLE );
			//float2 displace = ( float2 ) ( dx + OVERSAMPLECUBE * pow( dx, 3 ), dy + OVERSAMPLECUBE * pow( dy, 3 ) );
			float2 pos = coord + displace;

			if( pos.x < 0 || pos.x >= width || pos.y < 0 || pos.y >= height )
				continue;

			float4 val1 = read_imagef( colimg1, SAMPLER_BILINEAR, pos  + ( float2 ) ( 0.5f, 0.5f));
			float4 gval1 = read_imagef( gradimg1, SAMPLER_BILINEAR, pos  + ( float2 ) ( 0.5f, 0.5f));

//			float w1 = exp( -dot( fabs( valcenter.xyz - val1.xyz ), ( float3 ) 1.0f ) * ( smoothstep( 0.0f, 28.0f, length( displace ) ) * 1.0f * COLORWEIGHT + 5.0f ) );

			float w1 = exp( -dot( fabs( valcenter.xyz - val1.xyz ), ( float3 ) 1.0f ) * COLORWEIGHT );
			pos = Mat3_MulVecProj2( &mat, pos );
			if( pos.x < 0 || pos.x >= width )
				continue;

			wsum1 += w1;

			float4 val2 = read_imagef( colimg2, SAMPLER_BILINEAR, pos  + ( float2 ) ( 0.5f, 0.5f));
			float4 gval2 = read_imagef( gradimg2, SAMPLER_BILINEAR, pos  + ( float2 ) ( 0.5f, 0.5f));

			float C = COLORGRADALPHA * dot( fmin( fabs( ( val1 - val2 ).xyz ), COLORMAXDIFF ), ( float3 ) 1.0f ) + ( 1.0f - COLORGRADALPHA ) * dot( fmin( fabs( ( gval1 - gval2 ) ), GRADMAXDIFF ), ( float4 ) 1.0f );

			ret1 += w1 * C;
		}
	}

	if( wsum1 <= 1.1f )
		return 1e5f;
	return ret1 / wsum1;
}

kernel void pmsfm_init( write_only global float8* output, int width, int height )
{
	RNG rng;
	const int2 coord = ( int2 ) ( get_global_id( 0 ), get_global_id( 1 ) );
	const float2 coordf = ( float2 ) ( get_global_id( 0 ), get_global_id( 1 ) );

	if( coord.x >= width || coord.y >= height )
		return;

	RNG_init( &rng, coord.y * width + coord.x, 8 );
	float8 ret = pmsfm_state_init( &rng );
	output[ width * coord.y + coord.x ] = ret;
}

kernel void pmsfm_propagate( write_only global float8* output, read_only global float8* old,
							 read_only image2d_t img1, read_only image2d_t img2,
							 read_only image2d_t gimg1, read_only image2d_t gimg2,
							 read_only Mat3 Kdst, read_only Mat3 Ksrc, const int patchsize, const int iter )
{
	RNG rng;
	const int width = get_image_width( img1 );
	const int height = get_image_height( img1 );
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
    const int lw = get_local_size( 0 );
    const int lh = get_local_size( 1 );
	const int2 base = ( int2 )( get_group_id( 0 ) * lw - PROPSIZE, get_group_id( 1 ) * lh - PROPSIZE );
	const int2 coord = ( int2 ) ( get_global_id( 0 ), get_global_id( 1 ) );
	const float2 coordf = ( float2 ) ( get_global_id( 0 ), get_global_id( 1 ) );

	local float8 buf[ 16 + 2 * PROPSIZE ][ 16 + 2 * PROPSIZE ];
	float8 self, neighbour;
	float  cost, ncost;

	for( int y = ly; y < lh + 2 * PROPSIZE; y += lh ) {
		for( int x = lx; x < lw + 2 * PROPSIZE; x += lw ) {
			int2 spos = base + ( int2 ) ( x, y );
			spos.x = clamp( spos.x, 0, width - 1 );
			spos.y = clamp( spos.y, 0, height - 1 );
			buf[ y ][ x ] = old[ width * spos.y + spos.x ];
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );

	if(	gx >= width || gy >= height )
		return;

/*	Mat3 Kdst, Ksrc;
Kdst.m[0] = ( float3 ) (525.0f, 0.0f, 319.5 );
Kdst.m[1] = ( float3 ) (0.0f, 525.0, 239.5 );
Kdst.m[2] = ( float3 ) (0.0f, 0.0f,  1.0f );

 	Ksrc.m[ 0 ] =( float3 ) (    0.00190480f  ,         0.0f ,   -0.60857 );
 	Ksrc.m[ 1 ] =( float3 ) (            0.0f  , 0.00190480f ,   -0.45619 );
 	Ksrc.m[ 2 ] =( float3 ) (            0.0f  ,         0.0f ,         1.0f ) ;*/


	self = buf[ ly + PROPSIZE ][ lx + PROPSIZE ];
	cost  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, self, &Kdst, &Ksrc, patchsize );

	RNG_init( &rng, ( coord.y * width + coord.x ) + iter, NUMRNDTRIES * 8 + NUMRNDSAMPLE * 2 );

	// sample the pmsfm_state of the neighbours
	for( int py = -PROPSIZE; py <= PROPSIZE; py++ ) {
		for( int px = -PROPSIZE; px <= PROPSIZE; px++ ) {

			if( px == 0 && py == 0 )
				continue;

			neighbour = buf[ ly + PROPSIZE + py ][ lx + PROPSIZE + px ];
			ncost  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, &Kdst, &Ksrc, patchsize );

			if( ncost <= cost  ) {
				self = neighbour;
				cost = ncost;
			}
		}
	}
	// rand neighbourhood tries
	for( int i = 0; i < NUMRNDSAMPLE; i++ ) {
		    int2 spos = ( int2 )( gx, gy ) + ( int2 )( RNG_float(&rng) * 10.0f + 0.5f, RNG_float(&rng) * 10.0f + 0.5f );
			spos.x = clamp( spos.x, 0, width - 1 );
			spos.y = clamp( spos.y, 0, height - 1 );

			neighbour = old[ width * spos.y + spos.x ];
			ncost  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, &Kdst, &Ksrc, patchsize );
			if( ncost <= cost  ) {
				self = neighbour;
				cost = ncost;
			}
	}
	// random try
	neighbour = pmsfm_state_init( &rng );
	ncost  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, &Kdst, &Ksrc, patchsize );

	if( ncost <= cost  ) {
		self = neighbour;
		cost = ncost;
	}

	// randomized refinement
	for( int i = 0; i < NUMRNDTRIES - 1; i++ ) {
		neighbour = pmsfm_state_refine( &rng, self );
		ncost  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, &Kdst, &Ksrc, patchsize );

		if( ncost <= cost  ) {
			self = neighbour;
			cost = ncost;
		}
	}

	output[ width * gy + gx ] = self;
}

kernel void pmsfm_consistency( write_only image2d_t output, read_only image2d_t left, read_only image2d_t right )
{
	int2 coord;
	const int width = get_image_width( output );
	const int height = get_image_height( output );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	// TODO
}


kernel void pmsfm_normalmap( write_only image2d_t normalmap, read_only global float8* states )

{
	int2 coord;
	const int width = get_image_width( normalmap );
	const int height = get_image_height( normalmap );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	float8 state = states[ width * coord.y + coord.x ];
	float4 val;
    val.xyz	= pmsfm_state_to_normal( state );
	val.xy = val.xy * 0.5f + ( float2 ) 0.5f;
	val.w   = 1.0f;
	write_imagef( normalmap, coord, val );
}

kernel void pmsfm_depthmap( write_only image2d_t depthmap, read_only global float8* states )
{
	int2 coord;
	const int width = get_image_width( depthmap );
	const int height = get_image_height( depthmap );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	float8 state = states[ width * coord.y + coord.x ];
	float val    = pmsfm_state_to_unitdepth( state );

	write_imagef( depthmap, coord, val );
}

kernel void pmsfm_smooth( write_only global float8* out, read_only global float8* in, int width, int height )
{
	int2 coord;
	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
    const int lw = get_local_size( 0 );
    const int lh = get_local_size( 1 );
	const int2 base = ( int2 )( get_group_id( 0 ) * lw - PROPSIZE, get_group_id( 1 ) * lh - PROPSIZE );

	local float8 buf[ 16 + 2 * PROPSIZE ][ 16 + 2 * PROPSIZE ];

	for( int y = ly; y < lh + 2 * PROPSIZE; y += lh ) {
		for( int x = lx; x < lw + 2 * PROPSIZE; x += lw ) {
			int2 spos = base + ( int2 ) ( x, y );
			spos.x = clamp( spos.x, 0, width - 1 );
			spos.y = clamp( spos.y, 0, height - 1 );
			buf[ y ][ x ] = in[ width * spos.y + spos.x ];
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );

	if( coord.x >= width || coord.y >= height )
		return;

	float8 avg = ( float8 ) 0;
	for( int py = -PROPSIZE; py <= PROPSIZE; py++ ) {
		for( int px = -PROPSIZE; px <= PROPSIZE; px++ ) {
			float8 tmp = buf[ ly + PROPSIZE + py ][ lx + PROPSIZE + px ];
			tmp.s345 = normalize( tmp.s345 );
			avg += tmp;
		}
	}

	avg /= ( float8 ) ( ( 2 * PROPSIZE + 1 ) * ( 2 * PROPSIZE + 1 ) );

	float8 state = buf[ ly + PROPSIZE ][ lx + PROPSIZE ];
	state.s012 = avg.s012;
	state.s345 = length( state.s345 ) * normalize( avg.s345 );
	out[ width * coord.y + coord.x ] = state;
}


kernel void pmsfm_rotmap( write_only image2d_t depthmap, read_only global float8* states )
{
	int2 coord;
	const int width = get_image_width( depthmap );
	const int height = get_image_height( depthmap );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	float8 state = states[ width * coord.y + coord.x ];
	float4 val;
	val.xyz = ( state.s012 / ( float3 ) ROTMAX ) * 0.5f + 0.5f;
	val.w = 1.0f;

	write_imagef( depthmap, coord, val );
}

kernel void pmsfm_transmap( write_only image2d_t depthmap, read_only global float8* states )
{
	int2 coord;
	const int width = get_image_width( depthmap );
	const int height = get_image_height( depthmap );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	float8 state = states[ width * coord.y + coord.x ];
	float4 val;
	val.xyz = normalize( state.s345 ) * ( float3 ) 0.5f + ( float3 ) 0.5f;
	val.w = 1.0f;

	write_imagef( depthmap, coord, val );
}

kernel void pmsfm_fwdwarp( write_only image2d_t output, read_only image2d_t image, read_only global float8* states, const Mat3 Kdst, const Mat3 Ksrc )
{
	int2 coord;
	const int width = get_image_width( image );
	const int height = get_image_height( image );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	float8 state = states[ width * coord.y + coord.x ];
	float4 val = read_imagef( image, SAMPLER_NN, coord );

	Mat3 mat;
	pmsfm_state_to_matrix( state, &mat, &Kdst, &Ksrc );
	float2 pt = Mat3_MulVecProj2( &mat, ( float2 ) ( coord.x, coord.y ) );
	int2 coord2 = ( int2 )  ( round( pt.x ), round( pt.y ) );
	if( coord2.x >= 0 && coord2.x < width && coord2.y >= 0 && coord2.y < height )
		write_imagef( output, coord2, val );
}

