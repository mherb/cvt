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


#define TXMAX 8.0f
#define TYMAX 0.0f
#define TAU 15.0f
#define TAU2 1.0f
#define PROPSIZE 3
#define ALPHA 0.05f

float distEuclidean( const float4 a, const float4 b )
{
	return fast_distance( a, b );
}

float distL1( const float4 a, const float4 b )
{
	return dot( fabs( a - b ), ( float4 ) ( 0.333f, 0.333f, 0.333f, 0.0f ) ) ;
}

float distL1Trunc( const float4 a, const float4 b, const float trunc )
{
	return fmin( dot( fabs( a - b ), ( float4 ) ( 0.33f, 0.33f, 0.33f, 0.0f ) ), trunc );
}

float distL1GTrunc( const float4 a, const float4 b, const float2 ga, const float2 gb )
{
//	return ALPHA * fmin( dot( fabs( a - b ), ( float4 ) ( 0.333f, 0.333f, 0.333f, 0.0f ) ), 0.04f ) + 0.9f * fmin( dot( fabs( ga - gb ), ( float2 ) 0.5f ), 0.02f );
	return ALPHA * fmin( fast_length( a - b ), 0.04f * 3.0f ) + ( 1.0f - ALPHA ) * fmin( fast_length( ga - gb ), 0.02f * 3.0f );
}

float weight( const float4 a, const float4 b, const float tau, const float2 dist )
{
//	float distance = fast_length( dist );
	return exp( -fast_length(  ( a - b ).xyz ) * ( tau ) );// * exp( - distance * 0.15f );
}

float4 initStereo( mwc64x_state_t* rng )
{
	float4 n;
	n.x = ( MWC64X_NextFloat( rng ) - 0.5f ) * 2.0f;
	n.y = ( MWC64X_NextFloat( rng ) - 0.5f ) * 2.0f;
	n.z = 0.01f + MWC64X_NextFloat( rng ) * TXMAX;

	float len = length( n.xy );
	if( len > 1.0f )
		n.xy = n.xy / ( len );

	return ( float4 ) ( n.xyz, 0.0f );
}

float4 refineStereo( mwc64x_state_t* rng, const float4 state, float zmul, float nmul )
{
	float4 n;
	float z = state.z;
	n.x = state.x;
	n.y = state.y;

	z += ( MWC64X_NextFloat( rng ) - 0.5f ) * zmul;
	z = clamp( z, 0.01f, TXMAX );
	n.x += ( MWC64X_NextFloat( rng ) - 0.5f ) * nmul;
	n.y += ( MWC64X_NextFloat( rng ) - 0.5f ) * nmul;

	float len = length( n.xy );
	if( len > 1.0f )
		n.xy = n.xy / ( len );

	return ( float4 ) ( n.x, n.y, z, 0.0f );
}


float4 stateToNormal( const float4 state )
{
	float4 n;
	n.x = state.x;
	n.y = state.y;
	n.z = sqrt( 1.0f - n.x * n.x - n.y * n.y );
	n.w = 1.0f;
	n.xyz = n.xyz * 0.5 + 0.5f;
	return n;
}


struct Mat4 {
	float4 m[ 4 ];
};

float4 Mat4_MulVec( struct Mat4* mat, const float4 vec )
{
	return ( float4 ) ( dot( mat->m[ 0 ], vec ), dot( mat->m[ 1 ], vec ), dot( mat->m[ 2 ], vec ), dot( mat->m[ 3 ], vec ) );
}

float3 Mat4_MulVec3( struct Mat4* mat, const float3 vec )
{
	return ( float3 ) ( dot( mat->m[ 0 ].xyz, vec ),
					   dot( mat->m[ 1 ].xyz, vec ),
					   dot( mat->m[ 2 ].xyz, vec ) );
}

float3 Mat4_TransMulVec3( struct Mat4* mat, const float3 vec )
{
	return ( float3 ) ( dot( ( float3 ) ( mat->m[ 0 ].x, mat->m[ 1 ].x, mat->m[ 2 ].x ), vec ),
						dot( ( float3 ) ( mat->m[ 0 ].y, mat->m[ 1 ].y, mat->m[ 2 ].y ), vec ),
						dot( ( float3 ) ( mat->m[ 0 ].z, mat->m[ 1 ].z, mat->m[ 2 ].z ), vec ) );
}



float2 Mat4_MulVec3Proj2( struct Mat4* mat, const float2 vec )
{
	float3 hvec = ( float3 ) ( vec.x, vec.y, 1.0f );
	float3 tmp = ( float3 ) ( dot( mat->m[ 0 ].xyz, hvec ),
				   dot( mat->m[ 1 ].xyz, hvec ),
				   dot( mat->m[ 2 ].xyz, hvec ) );
	return ( float2 ) ( tmp.x / tmp.z, tmp.y / tmp.z );
}

void Mat4_Outer3( struct Mat4* dst, const float3 a, const float3 b )
{
	dst->m[ 0 ].xyz = a.x * b;
	dst->m[ 1 ].xyz = a.y * b;
	dst->m[ 2 ].xyz = a.z * b;
}

void Mat4_MatMul3( struct Mat4* dst, const struct Mat4* a, const struct Mat4* b )
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


void Mat4_MatMulTranspose3( struct Mat4* dst, const struct Mat4* a, const struct Mat4* b )
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

void Mat4_Add( struct Mat4* dst, struct Mat4* a, struct Mat4* b )
{
	dst->m[ 0 ] = a->m[ 0 ] + b->m[ 0 ];
	dst->m[ 1 ] = a->m[ 1 ] + b->m[ 1 ];
	dst->m[ 2 ] = a->m[ 2 ] + b->m[ 2 ];
	dst->m[ 3 ] = a->m[ 3 ] + b->m[ 3 ];
}


void Mat4_planeSweep( struct Mat4* output, struct Mat4* KDst, struct Mat4* TDst, struct Mat4* KInvSrc, struct Mat4* TSrc, float4 state )
{
	float3 tsrc = ( float3 )( TSrc->m[ 0 ].w, TSrc->m[ 1 ].w, TSrc->m[ 2 ].w );
	float3 tdst = ( float3 )( TDst->m[ 0 ].w, TDst->m[ 1 ].w, TDst->m[ 2 ].w );
	float3 tall;// = tdst - Mat4_TransMulVec3( TSrc, tsrc );
	float3 nd = ( float3 ) ( state.x, state.y, sqrt( 1.0f - state.x * state.x - state.y * state.y ) ) / state.z;
//	float3 nd = ( float3 ) ( 0.0f, 0.0f, 1.0f ) / state.z;

	/*
	   K_t * ( R_t^T * R_s + ( -R_t^T * t_t + R_s^T * t_s ) * normal/depth ) * K_s^-1
	 */
	struct Mat4 outer, tmp1, tmp2;

	Mat4_MatMulTranspose3( &tmp1, TDst, TSrc );
	tall = -Mat4_MulVec3( &tmp1, tsrc ) + tdst;
	Mat4_Outer3( &outer, tall, nd );
	Mat4_Add( &tmp2, &tmp1, &outer );
	Mat4_MatMul3( &tmp1, &tmp2, KInvSrc );
	Mat4_MatMul3( output, KDst, &tmp1 );
}

kernel void planeSweep( write_only image2d_t output, read_only image2d_t input, global read_only struct Mat4* mat, const int depth )
{
	const sampler_t samplerlin = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP| CLK_FILTER_LINEAR;
	const int2 coord = ( int2 ) ( get_global_id( 0 ), get_global_id( 1 ) );
	const float2 coordf = ( float2 ) ( get_global_id( 0 ), get_global_id( 1 ) );
	const int width = get_image_width( output );
	const int height = get_image_height( output );
	struct Mat4 H, KDst, TDst, KSrc1, TSrc1;
	float4 val;

	if( coord.x >= width || coord.y >= height )
		return;

	KDst     = mat[ 0 ];
    TDst     = mat[ 1 ];
    KSrc1	 = mat[ 2 ];
    TSrc1    = mat[ 3 ];

//	H = mat[ 0 ];

	Mat4_planeSweep( &H, &KDst, &TDst, &KSrc1, &TSrc1, ( float4 ) ( 0, 0, depth, 0 ) );
//	if( fabs( H.m[ 0 ].y ) < 1e-5f )
//		val = ( float4 ) ( 0.0f, 1.0f, 0.0f, 1.0f );
//	else
//		val = ( float4 ) ( 1.0f, 0.0f, 0.0f, 1.0f );
	val = read_imagef( input, samplerlin, Mat4_MulVec3Proj2( &H, coordf ) );
//	float4 val = read_imagef( input, samplerlin, coordf );
	write_imagef( output, coord, val );
}

kernel void patchmatchInit( write_only image2d_t matches,
						   read_only image2d_t img1, read_only image2d_t img2, read_only image2d_t img3,
						   read_only image2d_t gimg1, read_only image2d_t gimg2, read_only image2d_t gimg3,
						   global read_only struct Mat4* mat /* Kdst, Tdst, Kimg2^-1, Timg2, Kimg3^-1, Timg3 */,
						   const int patchsize )
{
	const sampler_t samplernn = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	const sampler_t samplerlin = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
	mwc64x_state_t rng;
	const int2 coord = ( int2 ) ( get_global_id( 0 ), get_global_id( 1 ) );
	const float2 coordf = ( float2 ) ( get_global_id( 0 ), get_global_id( 1 ) );
	const int width = get_image_width( img1 );
	const int height = get_image_height( img1 );
	struct Mat4 H1, H2, KInvDst, TDst, KSrc1, TSrc1, KSrc2, TSrc2;

	if( coord.x >= width || coord.y >= height )
		return;

	KInvDst  = mat[ 0 ];
    TDst     = mat[ 1 ];
    KSrc1	 = mat[ 2 ];
    TSrc1    = mat[ 3 ];
    KSrc2	 = mat[ 4 ];
    TSrc2    = mat[ 5 ];


	MWC64X_SeedStreams( &rng, 0, 0 );
	MWC64X_Skip( &rng, ( coord.y * width + coord.x ) * 3 );

	float4 ret = initStereo( &rng );
	ret.w = 0;

	float4 valcenter = read_imagef( img1, samplerlin, coordf );
	float  wsum = 0;

	Mat4_planeSweep( &H1, &KSrc1, &TSrc1, &KInvDst, &TDst, ret );
	Mat4_planeSweep( &H2, &KSrc2, &TSrc2, &KInvDst, &TDst, ret );

	for( int dy = -patchsize; dy <= patchsize; dy++ ) {
		for( int dx = -patchsize; dx <= patchsize; dx++ ) {
			float4 val1 = read_imagef( img1, samplerlin, coordf + ( float2 ) ( dx, dy ) );
			float2 gval1 = read_imagef( gimg1, samplerlin, coordf + ( float2 ) ( dx, dy ) ).xy;

			float2 pt = Mat4_MulVec3Proj2( &H1, coordf  + ( float2 ) ( dx, dy ) );
			float4 val2 = read_imagef( img2, samplerlin, pt );
			float2 gval2 = read_imagef( gimg2, samplerlin, pt ).xy;

			pt = Mat4_MulVec3Proj2( &H2, coordf  + ( float2 ) ( dx, dy ) );
			float4 val3 = read_imagef( img3, samplerlin, pt );
			float2 gval3 = read_imagef( gimg3, samplerlin, pt ).xy;

			float w = weight( valcenter, val1, TAU,  ( float2 ) ( dx, dy ) );
			float w2 = 1.0f;//weight( valcenter, val2, TAU2,  ( float2 ) ( dx, dy ) );
			float w3 = 1.0f;//weight( valcenter, val3, TAU2,  ( float2 ) ( dx, dy ) );
			wsum += w * w2 + w * w3;
			ret.w += w * w2 * distL1GTrunc( val1, val2, gval1, gval2 );
			ret.w += w * w3 * distL1GTrunc( val1, val3, gval1, gval3 );
//			ret.w += w * 0.25f * distL1GTrunc( val2, val3, gval2, gval3 );
		}
	}
	ret.w /= wsum;
	write_imagef( matches, coord, ret );
}

#define LOCALBUF 1

kernel void patchmatchPropagate( write_only image2d_t matches, read_only image2d_t oldmatches,
							     read_only image2d_t img1, read_only image2d_t img2, read_only image2d_t img3,
						         read_only image2d_t gimg1, read_only image2d_t gimg2, read_only image2d_t gimg3,
						         global read_only struct Mat4* mat /* Kdst, Tdst, Kimg2^-1, Timg2, Kimg3^-1, Timg3 */,
								const int iter, const int patchsize
#if LOCALBUF
	,local float4* bufimg, local float2* gbufimg )
#else
	)
#endif
{
	const sampler_t samplernn = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	const sampler_t samplerlin = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
	const int width = get_image_width( img1 );
	const int height = get_image_height( img1 );
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
    const int lw = get_local_size( 0 );
    const int lh = get_local_size( 1 );
	const int bstride = lw + 2 * patchsize;
	const int2 base = ( int2 )( get_group_id( 0 ) * lw - PROPSIZE, get_group_id( 1 ) * lh - PROPSIZE );
	const int2 baseimg = ( int2 )( get_group_id( 0 ) * lw - patchsize, get_group_id( 1 ) * lh - patchsize );
	const int2 coord = ( int2 ) ( get_global_id( 0 ), get_global_id( 1 ) );
	const float2 coordf = ( float2 ) ( get_global_id( 0 ), get_global_id( 1 ) );
	float4 neighbour;
	mwc64x_state_t rng;
	local float4 buf[ 16 + 2 * PROPSIZE ][ 16 + 2 * PROPSIZE ];
	struct Mat4 H1, H2, KInvDst, TDst, KSrc1, TSrc1, KSrc2, TSrc2;

	for( int y = ly; y < lh + 2 * PROPSIZE; y += lh ) {
		for( int x = lx; x < lw + 2 * PROPSIZE; x += lw ) {
			buf[ y ][ x ] = read_imagef( oldmatches, samplernn, base + ( int2 )( x, y ) );
		}
	}

#if LOCALBUF
	for( int y = ly; y < lh + 2 * patchsize; y += lh ) {
		for( int x = lx; x < lw + 2 * patchsize; x += lw ) {
			bufimg[ y * bstride + x ] = read_imagef( img1, samplerlin, baseimg + ( int2 )( x, y ) );
			gbufimg[ y * bstride + x ] = read_imagef( gimg1, samplerlin, baseimg + ( int2 )( x, y ) ).xy;
		}
	}
#endif

	barrier( CLK_LOCAL_MEM_FENCE );

	if(	gx >= width || gy >= height )
		return;

	KInvDst  = mat[ 0 ];
    TDst     = mat[ 1 ];
    KSrc1	 = mat[ 2 ];
    TSrc1    = mat[ 3 ];
    KSrc2	 = mat[ 4 ];
    TSrc2    = mat[ 5 ];

	MWC64X_SeedStreams( &rng, iter, 0 );
	MWC64X_Skip( &rng, ( gy * width + gx ) * iter * 3 * 2 );

	float4 self = buf[ ly + PROPSIZE ][ lx + PROPSIZE ];
#if LOCALBUF
	float4 valcenter = bufimg[ ( ly + patchsize ) * bstride + ( lx + patchsize ) ];
#else
	float4 valcenter = read_imagef( img1, samplerlin, coordf );
#endif


//	for( int iy = 0; iy < lh + PROPSIZE * 2; iy++ ) {
//		for( int ix = 0; ix < lw + PROPSIZE * 2; ix++ ) {
	for( int iy = ly + PROPSIZE - 1; iy < ly + PROPSIZE + 2; iy++ ) {
		for( int ix = lx + PROPSIZE - 1; ix < lx + PROPSIZE  + 2; ix++ ) {

			if( ix == lx + PROPSIZE && iy == ly + PROPSIZE )
				continue;

			neighbour = buf[ iy ][ ix ];
			if( neighbour.w > self.w )
				neighbour = refineStereo( &rng, neighbour, 3.0f, 1.0f );
//			else
//				neighbour = refineStereo( &rng, neighbour, 0.5f, 0.05f );
			neighbour.w = 0;
			Mat4_planeSweep( &H1, &KSrc1, &TSrc1, &KInvDst, &TDst, neighbour );
			Mat4_planeSweep( &H2, &KSrc2, &TSrc2, &KInvDst, &TDst, neighbour );



			float  wsum = 0;

			for( int dy = -patchsize; dy <= patchsize; dy++ ) {
				for( int dx = -patchsize; dx <= patchsize; dx++ ) {
#if LOCALBUF
					float4 val1 = bufimg[ ( ly + patchsize + dy ) * bstride + lx + dx + patchsize ];
					float2 gval1 = gbufimg[ ( ly + patchsize + dy ) * bstride + lx + dx + patchsize ];
#else
					float4 val1 = read_imagef( img1, samplerlin, coordf + ( float2 ) ( dx, dy ) );
					float2 gval1 = read_imagef( gimg1, samplerlin, coordf + ( float2 ) ( dx, dy ) ).xy;
#endif
					float2 pt = Mat4_MulVec3Proj2( &H1, coordf  + ( float2 ) ( dx, dy ) );
					float4 val2 = read_imagef( img2, samplerlin, pt );
					float2 gval2 = read_imagef( gimg2, samplerlin, pt ).xy;

					pt = Mat4_MulVec3Proj2( &H2, coordf  + ( float2 ) ( dx, dy ) );
					float4 val3 = read_imagef( img3, samplerlin, pt );
					float2 gval3 = read_imagef( gimg3, samplerlin, pt ).xy;

			float w = weight( valcenter, val1, TAU,  ( float2 ) ( dx, dy ) );
			float w2 = 1.0f;//weight( valcenter, val2, TAU2,  ( float2 ) ( dx, dy ) );
			float w3 = 1.0f;//weight( valcenter, val3, TAU2,  ( float2 ) ( dx, dy ) );
			wsum += w * w2 + w * w3;
			neighbour.w += w * w2 * distL1GTrunc( val1, val2, gval1, gval2 );
			neighbour.w += w * w3 * distL1GTrunc( val1, val3, gval1, gval3 );
//					neighbour.w += w * 0.25f * distL1GTrunc( val2, val3, gval2, gval3 );

				}
			}
			neighbour.w /= wsum;
			if( neighbour.w <= self.w  )
				self = neighbour;
		}
	}

	for( int i = 0; i < 3; i++ ) {

		if( self.w < 0.05 )
			neighbour = refineStereo( &rng, self, 0.05f, 0.05f );
		else
			neighbour = initStereo( &rng );

		neighbour.w = 0;
		Mat4_planeSweep( &H1, &KSrc1, &TSrc1, &KInvDst, &TDst, neighbour );
		Mat4_planeSweep( &H2, &KSrc2, &TSrc2, &KInvDst, &TDst, neighbour );


		float wsum = 0;
		for( int dy = -patchsize; dy <= patchsize; dy++ ) {
			for( int dx = -patchsize; dx <= patchsize; dx++ ) {
#if LOCALBUF
					float4 val1 = bufimg[ ( ly + patchsize + dy ) * bstride + lx + dx + patchsize ];
					float2 gval1 = gbufimg[ ( ly + patchsize + dy ) * bstride + lx + dx + patchsize ];
#else
					float4 val1 = read_imagef( img1, samplerlin, coordf + ( float2 ) ( dx, dy ) );
					float2 gval1 = read_imagef( gimg1, samplerlin, coordf + ( float2 ) ( dx, dy ) ).xy;
#endif
					float2 pt = Mat4_MulVec3Proj2( &H1, coordf  + ( float2 ) ( dx, dy ) );
					float4 val2 = read_imagef( img2, samplerlin, pt );
					float2 gval2 = read_imagef( gimg2, samplerlin, pt ).xy;

					pt = Mat4_MulVec3Proj2( &H2, coordf  + ( float2 ) ( dx, dy ) );
					float4 val3 = read_imagef( img3, samplerlin, pt );
					float2 gval3 = read_imagef( gimg3, samplerlin, pt ).xy;

			float w = weight( valcenter, val1, TAU,  ( float2 ) ( dx, dy ) );
			float w2 = 1.0f;//weight( valcenter, val2, TAU2,  ( float2 ) ( dx, dy ) );
			float w3 = 1.0f;//weight( valcenter, val3, TAU2,  ( float2 ) ( dx, dy ) );
			wsum += w * w2 + w * w3;
			neighbour.w += w * w2 * distL1GTrunc( val1, val2, gval1, gval2 );
			neighbour.w += w * w3 * distL1GTrunc( val1, val3, gval1, gval3 );
//					neighbour.w += w * 0.25f * distL1GTrunc( val2, val3, gval2, gval3 );
			}
		}
		neighbour.w /= wsum;
		if( neighbour.w <= self.w )
			self = neighbour;
	}

	write_imagef( matches, ( int2 ) ( gx, gy ), self );
}

kernel void patchmatchApply( write_only image2d_t output, read_only image2d_t matches, read_only image2d_t img )
{
	const sampler_t samplernn = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const sampler_t samplerlin = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;
	int2 coord;
	float2 coord2;
	const int width = get_image_width( img );
	const int height = get_image_height( img );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	float4 match = read_imagef( matches, samplernn, coord );
	write_imagef( output, coord, read_imagef( img, samplerlin, ( float2 ) ( coord.x + match.x , coord.y + match.y ) ) );
}

kernel void patchmatchToFlow( write_only image2d_t output, read_only image2d_t matches, global read_only struct Mat4* mat  )
{
	const sampler_t samplernn = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	int2 coord;
	const int width = get_image_width( output );
	const int height = get_image_height( output );
	struct Mat4 Kinv;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	Kinv = mat[ 0 ];

	float4 state = read_imagef( matches, samplernn, coord );
	float3 nd = ( float3 ) ( state.x, state.y, sqrt( 1.0f - state.x * state.x - state.y * state.y ) );
//	float3 nd = ( float3 ) ( 0, 0, 1.0f );
	float4 val;
	val.xyz = ( state.z / ( dot( nd, ( float3 ) ( Mat4_MulVec3( &Kinv, ( float3 ) ( coord.x, coord.y, 1.0f ) ) ) ) ) ) / TXMAX;
//	val.xyz = state.w;
//	val = stateToNormal( state );
//	val.xyz =  ( state.z ) / TXMAX;
//	val.xyz = ( float3 ) ( state.xy*0.5f + 0.5f, state.z / TXMAX );
//	if( val.x < 0 || val.x > 1.0f )
//		val.xyz = ( float3 )( 1, 0, 0 );
	val.w = 1.0f;
	write_imagef( output, coord, val );
}

