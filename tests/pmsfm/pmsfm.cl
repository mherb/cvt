#import "../../cvt/cl/kernel/RNG.cl"
#import "../../cvt/cl/kernel/Matrix3.cl"

//#pragma SELECT_ROUNDING_MODE rtz

// values in meter
#define DEPTHMAX  10.0f
#define DEPTHMIN  0.5f

#define TRANSMAX  1.0f
//#define ROTMAX	  0.25f
//#define ROTYMAX ( ( 30.0f / 180.0f ) * M_PI )

#define ROTMAX ( ( 25.0f / 180.0f ) * M_PI )
#define NORMALCOMPMAX 0.95f

#define PROPSIZE 2
#define NUMRNDTRIES	 2
#define NUMRNDSAMPLE  2

#define RNDSAMPLERADIUS 20.0f

#define DEPTHREFINEMUL 0.1f
#define NORMALREFINEMUL 0.05f
#define TRANSREFINEMUL  0.01f
#define ROTREFINEMUL 0.5f

#define COLORWEIGHT 10.0f
#define COLORGRADALPHA 0.5f
#define COLORMAXDIFF 0.2f
#define GRADMAXDIFF 0.1f
#define OVERSAMPLE 1.0f
#define OVERSAMPLECUBE 0.01f

// #define USELOCALBUF  1

const sampler_t SAMPLER_NN		 = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
const sampler_t SAMPLER_BILINEAR = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

float8 pmsfm_state_init( RNG* rng )
{
	float8 ret;
	float z;
	float3 n;
	float3 rot;
	float3 trans;

	z = DEPTHMIN + RNG_float( rng ) * ( DEPTHMAX - DEPTHMIN  ) ;

	n.x = ( RNG_float( rng ) - 0.5f ) * 2.0f * NORMALCOMPMAX;
	n.y = ( RNG_float( rng ) - 0.5f ) * 2.0f * NORMALCOMPMAX;

	rot.x = ( RNG_float( rng ) - 0.5f ) * 2.0f * ROTMAX;
	rot.y = ( RNG_float( rng ) - 0.5f ) * 2.0f * ROTMAX;
	rot.z = ( RNG_float( rng ) - 0.5f ) * 2.0f * ROTMAX;
//	rot.y = sqrt( 1.0f - rot.x * rot.x - rot.z * rot.z );
//	rot  *= ( RNG_float( rng ) - 0.5f ) * 2.0f * ROTYMAX;

	trans.x = ( RNG_float( rng ) - 0.5f ) * 2.0f * TRANSMAX;
	trans.y = ( RNG_float( rng ) - 0.5f ) * 2.0f * TRANSMAX;
	trans.z = ( RNG_float( rng ) - 0.5f ) * 2.0f * TRANSMAX;
	trans = normalize( trans );

	trans /= ( float3 ) z;

	return ( float8 )( rot.x, rot.y, rot.z, trans.x, trans.y, trans.z, n.x, n.y );
}

float8 pmsfm_state_refine( RNG* rng, const float8 state )
{
	float8 ret;
	float z, len, rotlen;
	float3 n;
	float3 rot;
	float3 trans;

	z = len = 1.0f / length( state.s345 );
	z += ( RNG_float( rng ) - 0.5f ) * 2.0f * DEPTHREFINEMUL;
	z = clamp( z, DEPTHMIN, DEPTHMAX );

	n.xy = state.s67;
	n.x += ( RNG_float( rng ) - 0.5f ) * 2.0f * NORMALREFINEMUL;
	n.y += ( RNG_float( rng ) - 0.5f ) * 2.0f * NORMALREFINEMUL;
	n.x = clamp( n.x, -NORMALCOMPMAX, NORMALCOMPMAX );
	n.y = clamp( n.y, -NORMALCOMPMAX, NORMALCOMPMAX );

	rot   = state.s012;
//	rotlen = length( rot );
//	rot   = normalize( rot );
	rot.x += ( RNG_float( rng ) - 0.5f ) * 2.0f * ROTREFINEMUL;
	rot.y += ( RNG_float( rng ) - 0.5f ) * 2.0f * ROTREFINEMUL;
	rot.z += ( RNG_float( rng ) - 0.5f ) * 2.0f * ROTREFINEMUL;
	rot.x = clamp( rot.x, -ROTMAX, ROTMAX );
	rot.y = clamp( rot.y, -ROTMAX, ROTMAX );
	rot.z = clamp( rot.z, -ROTMAX, ROTMAX );
//	rot.y = sign( rot.y ) * sqrt( 1.0f - rot.x * rot.x - rot.z * rot.z );
//	rotlen += ( RNG_float( rng ) - 0.5f ) * 2.0f * ( 1.0f / 180.0f ) * M_PI;
//	rotlen = clamp( rotlen, -ROTYMAX, ROTYMAX );
//	rot  *= rotlen;

	trans = normalize( state.s345 );
	trans.x += ( RNG_float( rng ) - 0.5f ) * 2.0f * ( TRANSREFINEMUL );
	trans.y += ( RNG_float( rng ) - 0.5f ) * 2.0f * ( TRANSREFINEMUL );
	trans.z += ( RNG_float( rng ) - 0.5f ) * 2.0f * ( TRANSREFINEMUL );
//	trans.x = clamp( trans.x, -TRANSMAX/DEPTHMAX, TRANSMAX/DEPTHMAX );
//	trans.y = clamp( trans.y, -TRANSMAX/DEPTHMAX, TRANSMAX/DEPTHMAX );
//	trans.z = clamp( trans.z, -TRANSMAX/DEPTHMAX, TRANSMAX/DEPTHMAX );
	trans = normalize( trans );
	trans /= ( float3 ) z;

	return ( float8 )( rot.x, rot.y, rot.z, trans.x, trans.y, trans.z, n.x, n.y );
}

inline float3 pmsfm_state_to_normal( const float8 state )
{
	return ( float3 ) ( state.s6, state.s7, sqrt( 1.0f - state.s6 * state.s6 - state.s7 * state.s7 ) );
}

inline void pmsfm_state_to_matrix( const float8 state, Mat3f* matrix, const Mat3f* Kdst, const Mat3f* Ksrc )
{
	Mat3f rot, outer, tmp;
	// rotation matrix
	mat3f_rotation_transpose( &rot, state.s012 );
//	mat3f_axisangle_rotation( &tmp, state.s012 );
//	mat3f_transpose( &rot, &tmp );
	// translation
	float3 t = state.s345;
	// normal
	float3 n = ( float3 ) pmsfm_state_to_normal( state );
	// K_dst ( R^T + (1/d) (R^T t) n^T ) K_src
	mat3f_outer( &outer, mat3f_transform( &rot, t ), n );
	mat3f_add( &tmp, &rot, &outer );
	mat3f_mul( &rot,  Kdst, &tmp );
	mat3f_mul( matrix, &rot, Ksrc );
}

inline float8 pmsfm_state_viewprop( const float8 state )
{
	// TODO, if needed at all
	return ( float8 ) 0.0f;
}

inline float pmsfm_state_to_depth( const float8 state )
{
	return length( state.s345 );
}

inline float pmsfm_state_to_unitdepth( const float8 state )
{
	return length( state.s345 ) / DEPTHMAX;
}


inline float patch_eval_color_grad_weighted( read_only image2d_t colimg1, read_only image2d_t gradimg1,
											 read_only image2d_t colimg2, read_only image2d_t gradimg2,
											 const float2 coord, const float8 state, const Mat3f* Kdst, const Mat3f* Ksrc, const int patchsize )
{
	float wsum1 = 0;
	float ret1 = 0;
	int width = get_image_width( colimg2 );
	int height = get_image_height( colimg2 );
	Mat3f mat;
//	Mat3f mat2;

	mat3f_axisangle_rotation( &mat, state.s012 );
//	mat3f_transpose( &mat, &mat2 );
//	if( dot( mat3f_transform( &mat, state.s345 ), pmsfm_state_to_normal( state ) ) - 1.0f < 0.0f )
//		return 1e5f;
//	if( dot( state.s345, pmsfm_state_to_normal( state ) ) - 1.0f > 0.0f )
//		return 1e5f;

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
			pos = mat3f_transform_proj2( &mat, pos );
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
							 read_only Mat3f Kdst, read_only Mat3f Ksrc, const int patchsize, const int iter )
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

/*	Mat3f Kdst, Ksrc;
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
		    int2 spos = ( int2 )( gx, gy ) + ( int2 )( RNG_float(&rng) * RNDSAMPLERADIUS + 0.5f, RNG_float(&rng) * RNDSAMPLERADIUS + 0.5f );
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

kernel void pmsfm_depthmap( write_only image2d_t depthmap, read_only global float8* states, const Mat3f Ksrc )
{
	int2 coord;
	const int width = get_image_width( depthmap );
	const int height = get_image_height( depthmap );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	float8 state = states[ width * coord.y + coord.x ];

	float d = length( state.s345 ) / fabs( dot( pmsfm_state_to_normal( state ), mat3f_transform( &Ksrc, ( float3 ) ( coord.x, coord.y, 1.0f ) ) ) );

	float val = 0.01f / d;//pmsfm_state_to_unitdepth( state );

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
	val.xyz = ( state.s012 / ( float3 ) ROTMAX ) * ( float3 ) 0.5f + ( float3 ) 0.5f;
//	float roty = length( state.s012 );
//	val.xyz = ( normalize(state.s012) / ( float3 ) ROTMAX ) * 0.5f + 0.5f;
//	val.y   = ( roty / ROTYMAX ) * 0.5f + 0.5f;
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

kernel void pmsfm_fwdwarp( write_only image2d_t output, read_only image2d_t image, read_only global float8* states, const Mat3f Kdst, const Mat3f Ksrc )
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

	Mat3f mat;
	pmsfm_state_to_matrix( state, &mat, &Kdst, &Ksrc );
	float2 pt = mat3f_transform_proj2( &mat, ( float2 ) ( coord.x, coord.y ) );
#if 0
	pt -= ( float2 ) ( coord.x, coord.y );
	val.xy = pt;
	val.z = 0.0f;
	val.w = 1.0f;
	write_imagef( output, coord, val );
#else
	int2 coord2 = ( int2 )  ( round( pt.x ), round( pt.y ) );
	if( coord2.x >= 0 && coord2.x < width && coord2.y >= 0 && coord2.y < height )
		write_imagef( output, coord2, val );
#endif
}

