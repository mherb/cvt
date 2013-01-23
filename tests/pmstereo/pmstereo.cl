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


#define DEPTHMAX 80.0f
#define PROPSIZE 2
#define DEPTHREFINEMUL 1.0f
#define NORMALREFINEMUL 0.25f
#define NORMALCOMPMAX 0.5f
#define NUMRNDTRIES	 3

#define COLORWEIGHT 25.0f
#define COLORGRADALPHA 0.05f
#define COLORMAXDIFF 0.05f
#define GRADMAXDIFF 0.05f

// #define USELOCALBUF  1

const sampler_t SAMPLER_NN		 = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
const sampler_t SAMPLER_BILINEAR = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;


float4 nd_state_init( RNG* rng, const float2 coord )
{
	float z = RNG_float( rng ) * DEPTHMAX;
	float4 n;
	n.x = ( MWC64X_NextFloat( rng ) - 0.5f ) * 2.0f * NORMALCOMPMAX;
	n.y = ( MWC64X_NextFloat( rng ) - 0.5f ) * 2.0f * NORMALCOMPMAX;

	n.z = sqrt( 1.0f - n.x * n.x - n.y * n.y );

	return ( float4 ) ( - n.x / n.z, - n.y / n.z, ( n.x * coord.x + n.y * coord.y + n.z * z  ) / n.z, 0.0f );
}

float4 nd_state_refine( RNG* rng, const float4 state, const float2 coord )
{
	float4 n;
	float z = state.x * coord.x + state.y * coord.y + state.z;
	n.z = ( sqrt( 1.0f / ( state.x * state.x + state.y * state.y + 1.0f ) ) );
	n.x = -state.x * n.z;
	n.y = -state.y * n.z;

	z += ( RNG_float( rng ) - 0.5f ) * DEPTHREFINEMUL;
	z = clamp( z, 0.0f, DEPTHMAX );
	n.x += ( RNG_float( rng ) - 0.5f ) * NORMALREFINEMUL;
	n.y += ( RNG_float( rng ) - 0.5f ) * NORMALREFINEMUL;

	n.x = clamp( n.x, -NORMALCOMPMAX, NORMALCOMPMAX );
	n.y = clamp( n.y, -NORMALCOMPMAX, NORMALCOMPMAX );

	n.z = sqrt( 1.0f - n.x * n.x - n.y * n.y );

	return ( float4 ) ( - n.x / n.z, - n.y / n.z, ( n.x * coord.x + n.y * coord.y + n.z * z  ) / n.z, 0.0f );
}

float nd_state_transform( const float4 state, const float2 coord )
{
	return state.x * coord.x + state.y * coord.y + state.z;
}

float4 nd_state_to_color( const float4 state, const float2 coord )
{
	float4 n;
	n.z = rsqrt( state.x * state.x + state.y * state.y + 1.0f );
	n.x = -state.x / n.z;
	n.y = -state.y / n.z;
	n.xy = n.xy * 0.5f + 0.5f;
	n.z = ( state.x * coord.x + state.y * coord.y + state.z ) / DEPTHMAX;
	n.w = 1.0f;
	return n;
}

float4 nd_state_to_normal( const float4 state )
{
	float4 n;
	n.z = rsqrt( state.x * state.x + state.y * state.y + 1.0f );
	n.x = -state.x / n.z;
	n.y = -state.y / n.z;
	n.z = sqrt( 1.0f - n.x * n.x - n.y * n.y );
	n.xyz = n.xyz * 0.5f + 0.5f;
	n.w = 1.0f;
	return n;
}



#if 0
kernel float patch_eval_color_grad_weighted_localbuf( int2 lcoord, local float4* colorbuf, local float2* gradbuf, size_t bstride,
												      read_only image2d_t colimg2, read_only image2d_t gradimg2,
												      float2 coord, float4 state, const int patchsize )
{
}

#endif

float patch_eval_color_grad_weighted( read_only image2d_t colimg1, read_only image2d_t gradimg1,
									  read_only image2d_t colimg2, read_only image2d_t gradimg2,
									  const float2 coord, const float4 state, const int patchsize, const int lr )
{
	float wsum1 = 0;
	float ret1 = 0;
	float4 m1 = 0, m2 = 0, m12 = 0;
	float4 m1sq = 0, m2sq = 0;

	float4 valcenter = read_imagef( colimg1, SAMPLER_NN, coord );

	for( int dy = -patchsize; dy <= patchsize; dy++ ) {
		for( int dx = -patchsize; dx <= patchsize; dx++ ) {

			float2 pos = coord + ( float2 ) ( dx, dy );

			float4 val1 = read_imagef( colimg1, SAMPLER_BILINEAR, pos );
			float4 gval1 = read_imagef( gradimg1, SAMPLER_BILINEAR, pos );

			float w1 = exp( -fast_length( valcenter.xyz - val1.xyz ) * COLORWEIGHT );
			wsum1 += w1;

			// transform point
			float d = nd_state_transform( state, pos );
			pos.x += select( d, -d, lr );

			float4 val2 = read_imagef( colimg2, SAMPLER_BILINEAR, pos );
			float4 gval2 = read_imagef( gradimg2, SAMPLER_BILINEAR, pos );

			float C = COLORGRADALPHA * fmin( fast_length( ( val1 - val2 ).xyz ), COLORMAXDIFF ) + ( 1.0f - COLORGRADALPHA ) * fmin( fast_length( gval1 - gval2 ), GRADMAXDIFF );
			ret1 += w1 * C;

			m1 = m1 + val1 * w1;
			m2 = m2 + val2 * w1;
			m12 = m12 + val1 * val2 * w1;

			m1sq += val1 * val1 * w1;
			m2sq += val2 * val2 * w1;

		}
	}

	float wsum1sqr = wsum1 * wsum1;

//	return 1.0f * dot( fabs( ( float ) 1.0f - clamp( ( float4 ) 0.0f, ( float4 ) 1.0f, ( ( m12 / wsum1 - m1 * m2 / wsum1sqr ) / sqrt( max( 1e-8f, ( m1sq / wsum1 - m1 * m1 / wsum1sqr ) * ( m2sq / wsum1 - m2 * m2 / wsum1sqr ) ) ) ) ) ), ( float4 ) 0.25f ) + 0.0f * ret1 / wsum1;
	return ret1 / wsum1;
}

kernel void pmstereo_init( write_only image2d_t output, read_only image2d_t img1, read_only image2d_t img2, read_only image2d_t gimg1, read_only image2d_t gimg2, const int patchsize, const int lr )
{
	RNG rng;
	const int2 coord = ( int2 ) ( get_global_id( 0 ), get_global_id( 1 ) );
	const float2 coordf = ( float2 ) ( get_global_id( 0 ), get_global_id( 1 ) );
	const int width = get_image_width( img1 );
	const int height = get_image_height( img1 );

	if( coord.x >= width || coord.y >= height )
		return;

	RNG_init( &rng, coord.y * width + coord.x, 3 );

	float4 ret = nd_state_init( &rng, coordf );

	ret.w  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, ret, patchsize, lr );

	write_imagef( output, coord, ret );
}

kernel void pmstereo_propagate( write_only image2d_t output, read_only image2d_t old,
							    read_only image2d_t img1, read_only image2d_t img2,
								read_only image2d_t gimg1, read_only image2d_t gimg2, const int patchsize, const int lr, const int iter )
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

	local float4 buf[ 16 + 2 * PROPSIZE ][ 16 + 2 * PROPSIZE ];
	float4 self, neighbour;

	for( int y = ly; y < lh + 2 * PROPSIZE; y += lh ) {
		for( int x = lx; x < lw + 2 * PROPSIZE; x += lw ) {
			buf[ y ][ x ] = read_imagef( old, SAMPLER_NN, base + ( int2 )( x, y ) );
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );

	if(	gx >= width || gy >= height )
		return;

	self = buf[ ly + PROPSIZE ][ lx + PROPSIZE ];

	RNG_init( &rng, ( coord.y * width + coord.x ) + iter, ( ( 2 * PROPSIZE + 1 ) * ( 2 * PROPSIZE + 1 ) - 1 ) + NUMRNDTRIES );

	// sample the nd_state of the neighbours
	for( int py = -PROPSIZE; py <= PROPSIZE; py++ ) {
		for( int px = -PROPSIZE; px <= PROPSIZE; px++ ) {

			if( px == 0 && py == 0 )
				continue;

			neighbour = buf[ ly + PROPSIZE + py ][ lx + PROPSIZE + px ];
			neighbour.w  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, patchsize, lr );

			if( neighbour.w <= self.w  )
				self = neighbour;
		}
	}
	// randomized refinement
	for( int i = 0; i < NUMRNDTRIES - 1; i++ ) {
		neighbour = nd_state_refine( &rng, self, coordf );
		neighbour.w  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, patchsize, lr );

		if( neighbour.w <= self.w  )
			self = neighbour;
	}
	// random try
	neighbour = nd_state_init( &rng, coordf );
	neighbour.w  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, patchsize, lr );

	if( neighbour.w <= self.w  )
		self = neighbour;

	write_imagef( output, coord, self );
}

#define VIEWSAMPLES 2

typedef struct {
	int n;
	float4 value[ VIEWSAMPLES ];// __attribute__ ((packed));
} VIEWPROP_t;

kernel void pmstereo_viewbuf_clear( global VIEWPROP_t* vbuf, const int width, const int height )
{
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );

	if(	gx >= width || gy >= height )
		return;

	vbuf[ width * gy + gx ].n = 0;
}

kernel void pmstereo_propagate_view( write_only image2d_t output, read_only image2d_t old,
							    read_only image2d_t img1, read_only image2d_t img2,
								read_only image2d_t gimg1, read_only image2d_t gimg2, const int patchsize, const int lr, const int iter,
							    global VIEWPROP_t* viewin, global VIEWPROP_t* viewout )
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

	local float4 buf[ 16 + 2 * PROPSIZE ][ 16 + 2 * PROPSIZE ];
	float4 self, neighbour;

	for( int y = ly; y < lh + 2 * PROPSIZE; y += lh ) {
		for( int x = lx; x < lw + 2 * PROPSIZE; x += lw ) {
			buf[ y ][ x ] = read_imagef( old, SAMPLER_NN, base + ( int2 )( x, y ) );
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );

	if(	gx >= width || gy >= height )
		return;

	self = buf[ ly + PROPSIZE ][ lx + PROPSIZE ];

	RNG_init( &rng, ( coord.y * width + coord.x ) + iter, ( ( 2 * PROPSIZE + 1 ) * ( 2 * PROPSIZE + 1 ) - 1 ) + NUMRNDTRIES );

	// sample the nd_state of the neighbours
	for( int py = -PROPSIZE; py <= PROPSIZE; py++ ) {
		for( int px = -PROPSIZE; px <= PROPSIZE; px++ ) {

			if( px == 0 && py == 0 )
				continue;

			neighbour = buf[ ly + PROPSIZE + py ][ lx + PROPSIZE + px ];
			neighbour.w  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, patchsize, lr );

			if( neighbour.w <= self.w  )
				self = neighbour;
		}
	}
	// random try
	neighbour = nd_state_init( &rng, coordf );
	neighbour.w  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, patchsize, lr );
	if( neighbour.w <= self.w  )
		self = neighbour;

	// try other view
#if 1
	int nview = min( viewin[ width * gy + gx ].n, ( int ) VIEWSAMPLES );
	for( int i = 0; i < nview; i++ ) {
		neighbour = viewin[ width * gy + gx ].value[ i ];
		neighbour.w  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, patchsize, lr );
		if( neighbour.w <= self.w  )
			self = neighbour;
	}
#endif

	// randomized refinement
	for( int i = 0; i < NUMRNDTRIES - 1; i++ ) {
		neighbour = nd_state_refine( &rng, self, coordf );
		neighbour.w  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, patchsize, lr );

		if( neighbour.w <= self.w  )
			self = neighbour;
	}

	// store view prop result
	// maybe inconsistent r/w access - but random anyway
#if 1
	float d = nd_state_transform( self, coordf );
	int disp = ( int ) ( select( d, -d, lr ) + coordf.x + 0.5f );
	if( disp >= 0 && disp < width ) {
		int nold = atomic_inc( &viewout[ coord.y * width + disp ].n );
		if( nold < VIEWSAMPLES )
			viewout[ coord.y * width + disp ].value[ nold ] = self;
	}
#endif

	write_imagef( output, coord, self );
}

kernel void pmstereo_depthmap( write_only image2d_t depthmap, read_only image2d_t old )
{
	int2 coord;
	const int width = get_image_width( depthmap );
	const int height = get_image_height( depthmap );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	float4 state = read_imagef( old, SAMPLER_NN, coord );
	float4 val;
	val.xyz = nd_state_transform( state, ( float2 ) ( coord.x, coord.y ) ) / DEPTHMAX;
	val.w = 1.0f;
	write_imagef( depthmap, coord, val );
}

kernel void pmstereo_normalmap( write_only image2d_t normalmap, read_only image2d_t old )

{
	int2 coord;
	const int width = get_image_width( normalmap );
	const int height = get_image_height( normalmap );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	float4 self = read_imagef( old, SAMPLER_NN, coord );
	float4 val = nd_state_to_normal( self );
	write_imagef( normalmap, coord, val );
}

kernel void pmstereo_colormap( write_only image2d_t normalmap, read_only image2d_t old )

{
	int2 coord;
	const int width = get_image_width( normalmap );
	const int height = get_image_height( normalmap );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	float4 self = read_imagef( old, SAMPLER_NN, coord );
	float4 val = nd_state_to_color( self, ( float2 ) ( coord.x, coord.y ) );
	write_imagef( normalmap, coord, val );
}
