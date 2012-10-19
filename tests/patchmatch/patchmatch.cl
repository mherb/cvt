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


#define TXMAX 70.0f
#define TYMAX 0.0f
#define TAU 15.0f
#define PROPSIZE 2
#define ALPHA 0.05f

//#define FLOW

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
//	return ALPHA * fmin( dot( fabs( a - b ), ( float4 ) ( 0.333f, 0.333f, 0.333f, 0.0f ) ), 0.04f ) + ( 1.0f - ALPHA ) * fmin( dot( fabs( ga - gb ), ( float2 ) 0.5f ), 0.02f );
	return ALPHA * fmin( fast_length( ( a - b).xyz ), 0.06f ) + ( 1.0f - ALPHA ) * fmin( fast_length( ga - gb ), 0.03f );
}

float weight( const float4 a, const float4 b, const float tau, const float2 dist )
{
	float distance = fast_length( dist );
//	float distance = dot( dist, dist );
//	return exp( - distance * 0.1f );
	return  exp( -fast_length(  a.xyz - b.xyz ) * ( tau ) ) * exp( - distance * 0.1f );
//	return  exp( -dot( fabs(  a - b ), ( float4 ) ( 0.333f, 0.333f, 0.333f, 0.0f ) ) * ( tau ) ) * exp( - distance * 0.15f );
}


float4 initFlow( mwc64x_state_t* rng )
{
	float4 ret;
	ret.x = ( MWC64X_NextFloat( rng ) - 0.5f ) * 2.0f * TXMAX;
	ret.y = ( MWC64X_NextFloat( rng ) - 0.5f ) * 2.0f * TYMAX;
	ret.z = 0.0f;
	ret.w = 0.0f;
	return ret;
}

float4 initStereo( mwc64x_state_t* rng, const float2 coord )
{
	float z = MWC64X_NextFloat( rng ) * TXMAX;
	float4 n;
	n.x = ( MWC64X_NextFloat( rng ) - 0.5f ) * 0.1f;
	n.y = ( MWC64X_NextFloat( rng ) - 0.5f ) * 0.1f;
//	n.z = ( MWC64X_NextFloat( rng ) ) * 1.0f;

//	n.xyz = normalize( n.xyz );
//	float len = length( n.xyz );
//	n.xyz = n.xyz / len;
//	float len = fast_length( n.xy );
//	if( len > 1.0f )
//		n.xy = n.xy / ( len );

#if 0
//	float nz = ( MWC64X_NextFloat( rng ) ) * 0.5f + 0.5f;
	float nz = ( MWC64X_NextFloat( rng ) - 0.5f ) * 2.0f;

	float len = length( ( float4 ) ( nx, ny, nz, 0.0f ) );
	nx /= len;
	ny /= len;
	nz /= len;
#else
	n.z = sqrt( 1.0f - n.x * n.x - n.y * n.y );
#endif

	return ( float4 ) ( - n.x / n.z, - n.y / n.z, ( n.x * coord.x + n.y * coord.y + n.z * z  ) / n.z, 0.0f );
}

float2 transformFlow( const float4 state, const float2 coord )
{
	return coord + state.xy;
}

float2 transformStereo( const float4 state, const float2 coord )
{
	return ( float2 ) ( state.x * coord.x + state.y * coord.y + state.z, 0 );
}

float4 refineStereo( mwc64x_state_t* rng, const float4 state, const float2 coord, float mul )
{
	float4 n;
	float z = transformStereo( state, coord ).x;
	n.z = ( sqrt( 1.0f / ( state.x * state.x + state.y * state.y + 1.0f ) ) );
	n.x = -state.x * n.z;
	n.y = -state.y * n.z;

//	z = fmin( fmax( z + ( MWC64X_NextFloat( rng ) - 0.5f ) * 1.0f, 0 ), TXMAX );
	z += ( MWC64X_NextFloat( rng ) - 0.5f ) * 1.0f * mul;
	z = clamp( z, 0.0f, TXMAX );
	n.x += ( MWC64X_NextFloat( rng ) - 0.5f ) * 0.5f * mul;
	n.y += ( MWC64X_NextFloat( rng ) - 0.5f ) * 0.5f * mul;
//	n.z += ( MWC64X_NextFloat( rng ) - 0.5f ) * 0.5f;

	n.x = clamp( n.x, -0.75f, 0.75f );
	n.y = clamp( n.y, -0.75f, 0.75f );

//	float len = fast_length( n.xy );
//	if( len > 1.0f )
//		n.xy = n.xy / ( len );

//	nz += ( MWC64X_NextFloat( rng ) - 0.5f ) * 0.5f;
#if 0
	float len = length( ( float4 ) ( nx, ny, nz, 0.0f ) );
	nx /= len;
	ny /= len;
	nz /= len;
#else
	n.z = sqrt( 1.0f - n.x * n.x - n.y * n.y );
#endif
	return ( float4 ) ( - n.x / n.z, - n.y / n.z, ( n.x * coord.x + n.y * coord.y + n.z * z  ) / n.z, 0.0f );
}

float4 stateToNormal( const float4 state )
{
	float4 n;
	n.z = rsqrt( state.x * state.x + state.y * state.y + 1.0f );
	n.x = -state.x / n.z;
	n.y = -state.y / n.z;
	n.w = 1.0f;
	n.xyz = n.xyz * 0.5 + 0.5f;
	return n;
}

float4 stateToNormal2( const float4 state )
{
	float4 n;
	n.z = rsqrt( state.x * state.x + state.y * state.y + 1.0f );
	n.x = -state.x / n.z;
	n.y = -state.y / n.z;
	n.w = 0.0f;
	return n;
}


float4 refineFlow( mwc64x_state_t* rng, const float4 state )
{
	float4 ret;
	ret.x = state.x + ( MWC64X_NextFloat( rng ) - 0.5f ) * 1.0f;
	ret.y = state.y + ( MWC64X_NextFloat( rng ) - 0.5f ) * 1.0f;
//	ret.y = clamp( state.y + ( MWC64X_NextFloat( rng ) - 0.5f ) * 1.0f, -2.0f, 2.0f );
	ret.z = 0.0f;
	ret.w = 0.0f;
	return ret;
}

float weightState( const float4 a, const float4 b, const float4 tau, float2 coord )
{
	float4 na, nb;
	float za = transformStereo( a, coord ).x;
	float zb = transformStereo( b, coord ).x;

	na.z = fabs( sqrt( 1.0f / ( a.x * a.x + a.y * a.y + 1.0f ) ) );
	na.x = -a.x * na.z;
	na.y = -a.y * na.z;

	nb.z = fabs( sqrt( 1.0f / ( b.x * b.x + b.y * b.y + 1.0f ) ) );
	nb.x = -b.x * nb.z;
	nb.y = -b.y * nb.z;

//	if( b.w < 0.05 && a.w < 0.05 )
//		return exp( -( fast_length( na.xyz - nb.xyz ) * tau.x + fabs( za - zb ) * tau.y ) );
//	else
//		return 1.0f;
		return exp( -( fabs( za - zb ) * tau.y )  );
}


kernel void patchmatchInit( write_only image2d_t matches, read_only image2d_t img1, read_only image2d_t img2, read_only image2d_t gimg1, read_only image2d_t gimg2, const int patchsize)
{
	const sampler_t samplernn = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	const sampler_t samplerlin = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
	mwc64x_state_t rng;
	const int2 coord = ( int2 ) ( get_global_id( 0 ), get_global_id( 1 ) );
	const float2 coordf = ( float2 ) ( get_global_id( 0 ), get_global_id( 1 ) );
	const int width = get_image_width( img1 );
	const int height = get_image_height( img1 );

	if( coord.x >= width || coord.y >= height )
		return;

	MWC64X_SeedStreams( &rng, 0, 0 );
	MWC64X_Skip( &rng, ( coord.y * width + coord.x ) * 3 );

#ifdef FLOW
	float4 ret = initFlow( &rng );
#else
	float4 ret = initStereo( &rng, coordf );
#endif

	float4 valcenter = read_imagef( img1, samplerlin, coordf );
//	float4 valcenter2 = read_imagef( img2, samplerlin, coordf + transformFlow ( ret, ( float2 ) ( 0, 0 ) ) );

	float  wsum = 0;
	for( int dy = -patchsize; dy <= patchsize; dy++ ) {
		for( int dx = -patchsize; dx <= patchsize; dx++ ) {
			float4 val1 = read_imagef( img1, samplerlin, coordf + ( float2 ) ( dx, dy ) );
			float2 gval1 = read_imagef( gimg1, samplerlin, coordf + ( float2 ) ( dx, dy ) ).xy;
#ifdef FLOW
			float4 val2 = read_imagef( img2, samplerlin, coordf + transformFlow ( ret, ( float2 ) ( dx, dy ) ) );
			float2 gval2 = read_imagef( gimg2, samplerlin, coordf + transformFlow ( ret, ( float2 ) ( dx, dy ) ) ).xy;
#else
			float4 val2 = read_imagef( img2, samplerlin, coordf - transformStereo( ret, coordf + ( float2 ) ( dx, dy ) ) + ( float2 ) ( dx, dy ) );
			float2 gval2 = read_imagef( gimg2, samplerlin, coordf - transformStereo( ret, coordf + ( float2 ) ( dx, dy ) ) + ( float2 ) ( dx, dy ) ).xy;
#endif
			float w = weight( valcenter, val1, TAU,  ( float2 ) ( dx, dy ) );// * weight( valcenter2, val2, TAU,  ( float2 ) ( dx, dy ) );
			wsum += w;
			ret.w += w * distL1GTrunc( val1, val2, gval1, gval2 );

		}
	}
	ret.w /= wsum;
	write_imagef( matches, coord, ret );
}

#define LOCALBUF 0

kernel void patchmatchPropagate( write_only image2d_t matches, read_only image2d_t oldmatches, read_only image2d_t img1, read_only image2d_t img2, read_only image2d_t gimg1, read_only image2d_t gimg2, int iter, const int patchsize
#if LOCALBUF
	,local float4* bufimg, local float2* gbufimg )
#else
	)
#endif
{
	const sampler_t samplernn = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const sampler_t samplerlin = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;
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

	MWC64X_SeedStreams( &rng, iter, 0 );
	MWC64X_Skip( &rng, ( gy * width + gx ) * iter * 4 * 2 );

	float4 self = buf[ ly + PROPSIZE ][ lx + PROPSIZE ];
#if LOCALBUF
	float4 valcenter = bufimg[ ( ly + patchsize ) * bstride + ( lx + patchsize ) ];
#else
	float4 valcenter = read_imagef( img1, samplerlin, coordf );
#endif


//	for( int iy = 0; iy < lh + PROPSIZE * 2; iy++ ) {
//		for( int ix = 0; ix < lw + PROPSIZE * 2; ix++ ) {
	for( int iy = ly; iy <= ly + PROPSIZE * 2; iy++ ) {
		for( int ix = lx; ix <= lx + PROPSIZE * 2; ix++ ) {

			if( ix == lx + PROPSIZE && iy == ly + PROPSIZE )
				continue;

			neighbour = buf[ iy ][ ix ];
//			if( neighbour.w > self.w )
//				continue;
//			neighbour = refineStereo( &rng, neighbour, coordf, 0.5f );
			neighbour.w = 0;

			float  wsum = 0;

//			float4 valcenter2 = read_imagef( img2, samplerlin, coordf + transformFlow ( neighbour, ( float2 ) ( 0, 0 ) ) );

			for( int dy = -patchsize; dy <= patchsize; dy++ ) {
				for( int dx = -patchsize; dx <= patchsize; dx++ ) {
#if LOCALBUF
					float4 val1 = bufimg[ ( ly + patchsize + dy ) * bstride + lx + dx + patchsize ];
					float2 gval1 = gbufimg[ ( ly + patchsize + dy ) * bstride + lx + dx + patchsize ];
#else
					float4 val1 = read_imagef( img1, samplerlin, coordf + ( float2 ) ( dx, dy ) );
					float2 gval1 = read_imagef( gimg1, samplerlin, coordf + ( float2 ) ( dx, dy ) ).xy;
#endif

#ifdef FLOW
					float4 val2 = read_imagef( img2, samplerlin, coordf + transformFlow( neighbour, ( float2 ) ( dx, dy ) ) );
					float2 gval2 = read_imagef( gimg2, samplerlin, coordf + transformFlow( neighbour, ( float2 ) ( dx, dy ) ) ).xy;
#else
					float4 val2 = read_imagef( img2, samplerlin, coordf - transformStereo( neighbour, coordf + ( float2 ) ( dx, dy ) ) + ( float2 ) ( dx, dy ) );
					float2 gval2 = read_imagef( gimg2, samplerlin, coordf - transformStereo( neighbour, coordf + ( float2 ) ( dx, dy ) ) + ( float2 ) ( dx, dy ) ).xy;
#endif
//					float4 state2 = buf[ ly + PROPSIZE + dy ][ lx + PROPSIZE + dx ];
			float w = weight( valcenter, val1, TAU,  ( float2 ) ( dx, dy ) );// * weight( valcenter2, val2, TAU,  ( float2 ) ( dx, dy ) );
					wsum += w;
					neighbour.w += w * distL1GTrunc( val1, val2, gval1, gval2 );

				}
			}
			neighbour.w /= wsum;
			if( neighbour.w <= self.w  )
				self = neighbour;
		}
	}

	for( int i = 0; i < 2; i++ ) {
#ifdef FLOW
		if( self.w < 0.05 )
			neighbour = refineFlow( &rng, self );
		else
			neighbour = initFlow( &rng );
#else
//		if( self.w < 0.05 )
			neighbour = refineStereo( &rng, self, coordf, 1.0f );
//		else
//			neighbour = initStereo( &rng, coordf );
#endif
		float wsum = 0;

//		float4 valcenter2 = read_imagef( img2, samplerlin, coordf + transformFlow ( neighbour, ( float2 ) ( 0, 0 ) ) );
		for( int dy = -patchsize; dy <= patchsize; dy++ ) {
			for( int dx = -patchsize; dx <= patchsize; dx++ ) {
#if LOCALBUF
					float4 val1 = bufimg[ ( ly + patchsize + dy ) * bstride + lx + dx + patchsize ];
					float2 gval1 = gbufimg[ ( ly + patchsize + dy ) * bstride + lx + dx + patchsize ];
#else
					float4 val1 = read_imagef( img1, samplerlin, coordf + ( float2 ) ( dx, dy ) );
					float2 gval1 = read_imagef( gimg1, samplerlin, coordf + ( float2 ) ( dx, dy ) ).xy;
#endif

#ifdef FLOW
					float4 val2 = read_imagef( img2, samplerlin, coordf + transformFlow( neighbour, ( float2 ) ( dx, dy ) ) );
					float2 gval2 = read_imagef( gimg2, samplerlin, coordf + transformFlow( neighbour, ( float2 ) ( dx, dy ) ) ).xy;
#else
					float4 val2 = read_imagef( img2, samplerlin, coordf - transformStereo( neighbour, coordf + ( float2 ) ( dx, dy ) ) + ( float2 ) ( dx, dy ) );
					float2 gval2 = read_imagef( gimg2, samplerlin, coordf - transformStereo( neighbour, coordf + ( float2 ) ( dx, dy ) ) + ( float2 ) ( dx, dy ) ).xy;
#endif
//					float4 state2 = buf[ ly + PROPSIZE + dy ][ lx + PROPSIZE + dx ];
			float w = weight( valcenter, val1, TAU,  ( float2 ) ( dx, dy ) );// * weight( valcenter2, val2, TAU,  ( float2 ) ( dx, dy ) );
					wsum += w;
					neighbour.w += w * distL1GTrunc( val1, val2, gval1, gval2 );
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

kernel void patchmatchToFlow( write_only image2d_t output, read_only image2d_t matches )
{
	const sampler_t samplernn = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	int2 coord;
	const int width = get_image_width( output );
	const int height = get_image_height( output );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	float4 match = read_imagef( matches, samplernn, coord );
#ifdef FLOW
//	write_imagef( output, coord, stateToNormal( match ) );
//	write_imagef( output, coord, ( float4 ) ( match.xy, 0, 0 ) );
	float4 val;
//	val = ( float4 ) ( match.xy, 0.0f, 1.0f );
	val.xyz = -match.x / TXMAX;
	val.w = 1.0f;
#else
	float4 val;
//	val.xyz = match.xyz;
	val.xyz =  transformStereo( match, ( float2 ) ( coord.x, coord.y ) ).x / TXMAX;
//	if( val.x < 0 || val.x > 1.0f )
//		val.xyz = ( float3 )( 1, 0, 0 );

//	val.xyz = match.w * 10.0f;

//	float4 matchx = read_imagef( matches, samplernn, coord + ( int2 ) ( 1, 0 ) );
//	float4 matchy = read_imagef( matches, samplernn, coord + ( int2 ) ( 0, 1 ) );

//	float dx =  transformStereo( matchx, ( float2 ) ( coord.x, coord.y ) ).x / TXMAX;
//	float dy =  transformStereo( matchy, ( float2 ) ( coord.x, coord.y ) ).x / TXMAX;
//	float d = val.x;
//	val.xyz = 0.25f * ( fabs( dx - d ) + fabs( dy - d ) );

//	match = stateToNormal2( match );
//	matchx = stateToNormal2( matchx );
//	matchy = stateToNormal2( matchy );

//	val.xyz = ( fast_length( match.xyz - matchx.xyz ) + fast_length( match.xyz - matchy.xyz ) ) * 0.001f;
//	val = stateToNormal( match );
	val.w = 1.0f;
#endif
	write_imagef( output, coord, val );
}

