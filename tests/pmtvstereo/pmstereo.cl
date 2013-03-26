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


#define DEPTHMAX 60.0f
#define PROPSIZE 3
#define DEPTHREFINEMUL 1.0f
#define NORMALREFINEMUL 0.05f
#define NORMALCOMPMAX 0.95f
#define NUMRNDTRIES	 3

#define COLORWEIGHT 26.0f
#define COLORGRADALPHA 0.05f
#define COLORMAXDIFF 0.04f
#define GRADMAXDIFF 0.01f
#define OVERSAMPLE 1.0f
#define OVERSAMPLECUBE 0.01f
#define VIEWSAMPLES 3

// #define USELOCALBUF  1

const sampler_t SAMPLER_NN		 = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
const sampler_t SAMPLER_BILINEAR = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;


float4 nd_state_viewprop( const float4 state, const float2 coord, const float _disp, const int lr );

float4 nd_state_init( RNG* rng, const float2 coord, int lr )
{
	float z = RNG_float( rng ) * DEPTHMAX;
	float4 n;
	n.x = ( MWC64X_NextFloat( rng ) - 0.5f ) * 2.0f * NORMALCOMPMAX;
	n.y = ( MWC64X_NextFloat( rng ) - 0.5f ) * 2.0f * NORMALCOMPMAX;

	n.z = sqrt( 1.0f - n.x * n.x - n.y * n.y );

//	return ( float4 ) ( - n.x / n.z, - n.y / n.z, ( n.x * coord.x + n.y * coord.y + n.z * z  ) / n.z, 0.0f );
	float4 ret = ( float4 ) ( 1.0f, 0.0f, 0.0f, 0.0f ) - ( float4 ) ( - n.x / n.z, - n.y / n.z, ( n.x * coord.x + n.y * coord.y ) / n.z + z, 0.0f );
	if( !lr )
		ret = nd_state_viewprop( ret, 0, 0, 0 );
//	ret = select( ( float4 ) ( 1.0f, 0.0f, 0.0f, 0.0f ) - ret,( float4 ) ( 1.0f, 0.0f, 0.0f, 0.0f ) + ret, ( int4 ) lr );
	return ret;
}

float4 nd_state_to_ref_normal_depth( const float4 state, const float2 coord, const int lr )
{
	float4 ret;
	float4 _state = state;
	if( !lr )
		_state = nd_state_viewprop( state, 0, 0, 0 );
	_state =  ( float4 ) ( 1.0f, 0.0f, 0.0f, 0.0f ) - _state;
	ret.w = _state.x * coord.x + _state.y * coord.y + _state.z;
	ret.z = rsqrt( _state.x * _state.x + _state.y * _state.y + 1.0f );
	ret.x = -_state.x * ret.z;
	ret.y = -_state.y * ret.z;

	return ret;
}

float4 nd_state_refine( RNG* rng, const float4 _state, const float2 coord, int lr )
{
	float4 n;
	float4 state = _state;
	if( !lr )
		state = nd_state_viewprop( state, 0, 0, 0 );
	state =  ( float4 ) ( 1.0f, 0.0f, 0.0f, 0.0f ) - state;
	float z = state.x * coord.x + state.y * coord.y + state.z;
	n.z = rsqrt( state.x * state.x + state.y * state.y + 1.0f );
	n.x = -state.x * n.z;
	n.y = -state.y * n.z;

	z += ( RNG_float( rng ) - 0.5f ) * DEPTHREFINEMUL;
	z = clamp( z, 0.0f, DEPTHMAX );
	n.x += ( RNG_float( rng ) - 0.5f ) * NORMALREFINEMUL;
	n.y += ( RNG_float( rng ) - 0.5f ) * NORMALREFINEMUL;

	n.x = clamp( n.x, -NORMALCOMPMAX, NORMALCOMPMAX );
	n.y = clamp( n.y, -NORMALCOMPMAX, NORMALCOMPMAX );

	n.z = sqrt( 1.0f - n.x * n.x - n.y * n.y );

//	return ( float4 ) ( - n.x / n.z, - n.y / n.z, ( n.x * coord.x + n.y * coord.y + n.z * z  ) / n.z, 0.0f );
	float4 ret = ( float4 ) ( - n.x / n.z, - n.y / n.z, ( n.x * coord.x + n.y * coord.y ) / n.z + z, 0.0f );
	ret = ( float4 ) ( 1.0f, 0.0f, 0.0f, 0.0f ) - ret;
	if( !lr )
		ret = nd_state_viewprop( state, 0, 0, 0 );
	return ret;
}


float4 nd_state_viewprop2( const float4 state )
{
	float4 n = state;
	n.x = 1.0f + n.x;
	return ( float4 ) ( ( 1.0f / n.x ) - 1.0f, -n.y / n.x, -n.z / n.x, 0.0f );
}

float4 nd_state_viewprop( const float4 state, const float2 coord, const float _disp, const int lr )
{
	float4 n = state;
//	float newx = //select( _disp, -_disp, lr ) + coord.x;
//	if( lr ) {
//		n.x = 1.0f - n.x;
//		return -( float4 ) ( ( 1.0f / n.x ), n.y / n.x, n.z / n.x - coord.x, 0.0f );
//	} else {
//		n.x = 1.0f + n.x;
		float4 ret = ( float4 ) ( ( 1.0f / n.x ), -n.y / n.x, -n.z / n.x, 0.0f );
		return ret;
//		return select( -ret, ret, ( int4 ) lr );
//	}
}

float2 nd_state_transform( const float4 state, const float2 coord )
{
	return ( float2 ) ( state.x * coord.x + state.y * coord.y + state.z, coord.y );
}

float4 nd_state_to_color( const float4 _state, const float2 coord )
{
	float4 n;
	float4 state =  ( float4 ) ( 1.0f, 0.0f, 0.0f, 0.0f ) - _state;
	n.z = rsqrt( state.x * state.x + state.y * state.y + 1.0f );
	n.x = -state.x / n.z;
	n.y = -state.y / n.z;
	n.xy = n.xy * 0.5f + 0.5f;
	n.z = ( state.x * coord.x + state.y * coord.y + state.z ) / DEPTHMAX;
	n.w = 1.0f;
	return n;
}

float4 nd_state_to_normal( const float4 _state )
{
	float4 n;
	float4 state =  ( float4 ) ( 1.0f, 0.0f, 0.0f, 0.0f ) - _state;
	n.z = rsqrt( state.x * state.x + state.y * state.y + 1.0f );
	n.x = -state.x / n.z;
	n.y = -state.y / n.z;
	n.z = sqrt( 1.0f - n.x * n.x - n.y * n.y );
	n.xyz = n.xyz * 0.5f + 0.5f;
	n.w = 1.0f;
	return n;
}


inline float patch_eval_color_grad_weighted( read_only image2d_t colimg1, read_only image2d_t gradimg1,
									  read_only image2d_t colimg2, read_only image2d_t gradimg2,
									  const float2 coord, const float4 state, const int patchsize, const int lr )
{
	float wsum1 = 0;
	float ret1 = 0;
	int width = get_image_width( colimg2 );
	int height = get_image_height( colimg2 );

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

			float w1 = exp( -dot( fabs( valcenter.xyz - val1.xyz ), ( float3 ) 1.0f ) * ( smoothstep( 0.0f, 28.0f, length( displace ) ) * 1.0f * COLORWEIGHT + 5.0f ) );// * exp( -fast_length( displace ) * 0.05f );

//			float w1 = exp( -dot( fabs( valcenter.xyz - val1.xyz ), ( float3 ) 1.0f ) * COLORWEIGHT );// * exp( -fast_length( displace ) * 0.05f );

			// transform point
	//		float d = nd_state_transform( state, pos );
	//		pos.x += select( d, -d, lr );
			pos = nd_state_transform( state, pos );
			if( pos.x < 0 || pos.x >= width )
				continue;

			wsum1 += w1;

#define TOGRAY(x) dot( x, grayWeight )

			float4 val2 = read_imagef( colimg2, SAMPLER_BILINEAR, pos  + ( float2 ) ( 0.5f, 0.5f));
			float4 gval2 = read_imagef( gradimg2, SAMPLER_BILINEAR, pos  + ( float2 ) ( 0.5f, 0.5f));

			float C = COLORGRADALPHA * dot( fmin( fabs( ( val1 - val2 ).xyz ), COLORMAXDIFF ), ( float3 ) 1.0f ) + ( 1.0f - COLORGRADALPHA ) * dot( fmin( fabs( ( gval1 - gval2 ) ), GRADMAXDIFF ), ( float4 ) 1.0f );

//			float C = log( 0.25f * exp(-1.0f * fmin( fast_length( ( val1 - val2 ).xyz ), COLORMAXDIFF  ) ) + 0.75f * exp( -1.0f * fmin( fast_length( ( gval1 - gval2 ) ), GRADMAXDIFF ) ) ) / -1.0f;

			ret1 += w1 * C;
		}
	}

	if( wsum1 <= 1.1f )
		return 1e5f;
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

	float4 ret = nd_state_init( &rng, coordf, lr );

	ret.w  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, ret, patchsize, lr );

	write_imagef( output, coord, ret );
}

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

float2 smoothDistance( const float4 current, const float4 other, const float4 smooth, const float2 coord, int lr )
{
  float2 ret;
  float4 a = nd_state_to_ref_normal_depth( current, coord, lr );
  float4 b = nd_state_to_ref_normal_depth( other, coord, lr );

  float4 da = a - smooth;
  ret.x = dot( da, da );

  float4 db = b - smooth;
  ret.y = dot( db, db );
}

kernel void pmstereo_propagate_view( write_only image2d_t output, read_only image2d_t old,
							    read_only image2d_t img1, read_only image2d_t img2,
								read_only image2d_t gimg1, read_only image2d_t gimg2, read_only image2d_t imsmoooth, const float theta,
								const int patchsize, const int lr, const int iter,
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
	float4 self, neighbour, smooth;

	for( int y = ly; y < lh + 2 * PROPSIZE; y += lh ) {
		for( int x = lx; x < lw + 2 * PROPSIZE; x += lw ) {
			buf[ y ][ x ] = read_imagef( old, SAMPLER_NN, base + ( int2 )( x, y ) );
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );

	if(	gx >= width || gy >= height )
		return;

	smooth = read_imagef( imsmoooth, SAMPLER_NN, coord );
	smooth = ( float4 ) ( smooth.x, smooth.y, sqrt( 1.0f - smooth.x * smooth.x - smooth.y * smooth.y ), smooth.z * DEPTHMAX );

	self = buf[ ly + PROPSIZE ][ lx + PROPSIZE ];

	RNG_init( &rng, ( coord.y * width + coord.x ) + iter, ( ( 2 * PROPSIZE + 1 ) * ( 2 * PROPSIZE + 1 ) - 1 ) + NUMRNDTRIES );

	// sample the nd_state of the neighbours
	for( int py = -PROPSIZE; py <= PROPSIZE; py++ ) {
		for( int px = -PROPSIZE; px <= PROPSIZE; px++ ) {

			if( px == 0 && py == 0 )
				continue;

			neighbour = buf[ ly + PROPSIZE + py ][ lx + PROPSIZE + px ];
			neighbour.w  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, patchsize, lr );

			float2 sdist = smoothDistance( self, neighbour, smooth, coordf, lr );
			if( self.w + theta * sdist.x <= neighbour.w + theta * sdist.y ) self = neighbour;
		}
	}
	// random try
	neighbour = nd_state_init( &rng, coordf, lr );
	neighbour.w  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, patchsize, lr );
	float2 sdist = smoothDistance( self, neighbour, smooth, coordf, lr );
	if( self.w + theta * sdist.x <= neighbour.w + theta * sdist.y ) self = neighbour;


	// try other view
#if 1
	int nview = min( viewin[ width * gy + gx ].n, ( int ) VIEWSAMPLES );
	for( int i = 0; i < nview; i++ ) {
		neighbour = viewin[ width * gy + gx ].value[ i ];
		neighbour.w  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, patchsize, lr );
		float2 sdist = smoothDistance( self, neighbour, smooth, coordf, lr );
		if( self.w + theta * sdist.x <= neighbour.w + theta * sdist.y ) self = neighbour;

	}
#endif

	// randomized refinement
	for( int i = 0; i < NUMRNDTRIES - 1; i++ ) {
		neighbour = nd_state_refine( &rng, self, coordf, lr );
		neighbour.w  = patch_eval_color_grad_weighted( img1, gimg1, img2, gimg2, coordf, neighbour, patchsize, lr );
		float2 sdist = smoothDistance( self, neighbour, smooth, coordf, lr );
		if( self.w + theta * sdist.x <= neighbour.w + theta * sdist.y ) self = neighbour;

	}

	// store view prop result
	// maybe inconsistent r/w access - but random anyway
#if 1
	float2 pos2 = nd_state_transform( self, coordf );
	int disp = ( int ) ( pos2.x + 0.5f );
	if( disp >= 0 && disp < width ) {
		int nold = atomic_inc( &viewout[ gy * width + disp ].n );
		if( nold < VIEWSAMPLES )
			viewout[ gy * width + disp ].value[ nold ] = nd_state_viewprop( self, coordf, 0, lr );
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
	val.xyz = fabs( nd_state_transform( state, ( float2 ) ( coord.x, coord.y ) ).x - ( float ) coord.x ) / DEPTHMAX;
	val.w = 1.0f;
	write_imagef( depthmap, coord, val );
}

kernel void pmstereo_consistency( write_only image2d_t output, read_only image2d_t left, read_only image2d_t right )
{
	int2 coord;
	const int width = get_image_width( output );
	const int height = get_image_height( output );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	float4 statel = read_imagef( left, SAMPLER_NN, coord );
	float2 coord2 = nd_state_transform( statel, ( float2 ) ( coord.x, coord.y ) );
	float4 stater;
	if( coord2.x < 0 || coord2.x >= width )
		stater = ( float4 ) 1e5f;
	else
		stater = read_imagef( right, SAMPLER_BILINEAR, coord2  + ( float2 ) ( 0.5f, 0.5f));
		//stater = read_imagef( right, SAMPLER_, ( int2 ) ( coord2.x + 0.5f, coord2.y ) );

	float4 val;
//    val = length( ( float2 ) ( coord.x, coord.y) - nd_state_transform( stater, coord2 ) )>1.0f?( float4 ) 0.0f : ( statel );
    val = fabs( ( float ) coord.x - nd_state_transform( stater, coord2 ).x )>=1.0f?( float4 ) 0.0f : ( statel );
//    val = length( statel - nd_state_viewprop( stater, 0, 0, 0 ) ) > 2.0f?( float4 ) 0.0f : ( statel );
	val.w = 1.0f;
	write_imagef( output, coord, val );
}


kernel void pmstereo_fill_depthmap( write_only image2d_t output, read_only image2d_t input, const float scale )
{
	const int2 coord = ( int2 ) ( get_global_id( 0 ), get_global_id( 1 ) );
	const int width = get_image_width( output );
	const int height = get_image_height( output );

	if( coord.x >= width || coord.y >= height )
		return;

	float4 val;
	float4 state = read_imagef( input, SAMPLER_NN, coord );

	if( length( state.xyz ) < 1e-1f ) {
		float4 left = ( float4 ) 0;
		int x = coord.x - 1;
		while( length( left.xyz ) < 1e-1f && x >= 0 ) {
			left = read_imagef( input, SAMPLER_NN, ( int2 ) ( x, coord.y ) );
			x--;
		}

		float4 right = ( float4 ) 0;
		x = coord.x + 1;
		while( length( right.xyz ) < 1e-1f && x < width ) {
			right = read_imagef( input, SAMPLER_NN, ( int2 ) ( x, coord.y ) );
			x++;
		}

		if( length( left.xyz ) < 1e-1f ) left.xyz = ( float3 ) -1e5f;
		if( length( right.xyz ) < 1e-1f ) right.xyz = ( float3 ) -1e5f;

		left.w = -( nd_state_transform( left, ( float2 ) ( coord.x, coord.y ) ).x - ( float ) coord.x );
		right.w = -( nd_state_transform( right, ( float2 ) ( coord.x, coord.y ) ).x - ( float ) coord.x );

		val.xyz = ( float3 ) fmin( left.w, right.w );
		val.w = 1.0f;

	} else {
		val.xyz = fabs( nd_state_transform( state, ( float2 ) ( coord.x, coord.y ) ).x - ( float ) coord.x );
		val.w = 1.0f;
	}

	val.xyz *= scale;
	write_imagef( output, coord, val );
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


kernel void pmstereo_normal_depth( write_only image2d_t output, read_only image2d_t input, int lr )

{
	int2 coord;
	const int width = get_image_width( output );
	const int height = get_image_height( output );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	float4 self = read_imagef( input, SAMPLER_NN, coord );
	float4 val = nd_state_to_ref_normal_depth( self, ( float2 ) ( coord.x, coord.y ), lr );
	val = ( float4 ) ( val.x, val.y, val.w / DEPTHMAX, 1.0f );
	write_imagef( output, coord, val );
}

kernel void pmstereo_lr_check( write_only image2d_t output, read_only image2d_t input1, read_only image2d_t input2, const float maxdiff, int lr )
{
	int2 coord;
	const int width = get_image_width( input1 );
	const int height = get_image_height( input1 );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	float dorig = read_imagef( input1, SAMPLER_NN, coord ).x;
	float d = DEPTHMAX * dorig;

	float2 coord2 = ( float2 ) ( coord.x, coord.y );
	coord2.x += select( d, -d, lr );
	float d2 = DEPTHMAX * read_imagef( input2, SAMPLER_BILINEAR, coord2 ).x;
	float4 out = ( float4 ) select( 0.0f, dorig /* * ( DEPTHMAX * 4.0f / 256.0f )*/, fabs( d - d2) < maxdiff );
	out.w = 1.0f;
	write_imagef( output, coord, out );
}
