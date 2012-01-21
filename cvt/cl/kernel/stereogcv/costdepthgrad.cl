#define COSTTHRESHOLD 0.05 //0.028f
#define COSTTHRESHOLDGRAD 0.01f //0.008f
#define ALPHA 0.10f

__kernel void stereogcv_costdepthgrad( __write_only image2d_t costout, __read_only image2d_t img0, __read_only image2d_t img1, __read_only image2d_t grad0, __read_only image2d_t grad1, __read_only global float4* proj, float depth )
{
	const float4 dotmul = ( float4 ) ( 0.3333f, 0.3333f, 0.3333f, 0.0f );
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;
	int2 coord;
	float2 coord2;
	float4 mat[ 3 ];
	float4 I0, I1, G0, G1, vec, Cout;
	const int width = get_image_width( img0 );
	const int height = get_image_height( img1 );
	float z;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	mat[ 0 ] = proj[ 0 ];
	mat[ 1 ] = proj[ 1 ];
	mat[ 2 ] = proj[ 2 ];

	vec = ( float4 )( coord.x, coord.y, depth, 1.0f );

	z = dot( vec, mat[ 2 ] );
	coord2.x = dot( vec, mat[ 0 ] ) / z;
	coord2.y = dot( vec, mat[ 1 ] ) / z;

	if( coord2.x < 0 || coord2.x >= width || coord2.y < 0 || coord2.y >= height || fabs( z ) <= 1e-6f  ) {
		Cout = ( float4 ) ( COSTTHRESHOLDGRAD + COSTTHRESHOLD );
		//I0 = read_imagef( img0, sampler, coord  );
		//G0 = read_imagef( grad0, sampler, coord );
		//I1 = ( float4 ) 0.5f;//read_imagef( img1, sampler, coord2 );
		//G1 = ( float4 ) 0.5f;//read_imagef( grad1, sampler, coord2 );
		//Cout = ( float4 ) mix( fmin( dot( fabs( I1 - I0 ), dotmul ), COSTTHRESHOLD ) , fmin( dot( fabs( G1 - G0 ), dotmul ), COSTTHRESHOLDGRAD ), ALPHA );
	} else {
//#define BLOCK
#ifdef BLOCK
		float c = 0.0f, wsum = 0.0f;
#define DSIZE 3
//		float delta[ DSIZE ] = { -2.0f, -1.0f, 0.0f, 1.0f, 2.0f };
//		float w[ DSIZE ] = { 0.25f, 0.25f, 0.5f, 0.25f, 0.25f };
		float delta[ DSIZE ] = { -1.0f, 0.0f, 1.0f };
		float w[ DSIZE ] = { 0.25f, 0.5f, 0.25f };

		for( int iy = 0; iy < DSIZE; iy++ ) {
			for( int ix = 0; ix < DSIZE; ix++ ) {
				I0 = read_imagef( img0, sampler, coord + ( float2 ) ( delta[ ix ], delta[ iy ] ) );
				G0 = read_imagef( grad0, sampler, coord  + ( float2 ) ( delta[ ix ], delta[ iy ] ));
				I1 = read_imagef( img1, sampler, coord2  + ( float2 ) ( delta[ ix ], delta[ iy ] ));
				G1 = read_imagef( grad1, sampler, coord2  + ( float2 ) ( delta[ ix ], delta[ iy ] ));
				c +=  w[ ix ] * w[ iy ] * mix( fmin( dot( fabs( I1 - I0 ), dotmul ), COSTTHRESHOLD ) , fmin( dot( fabs( G1 - G0 ), dotmul ), COSTTHRESHOLDGRAD ), ALPHA );
				//c += w[ ix ] * w[ iy ] * ( mix( fmin( fast_length( I1 - I0 ), COSTTHRESHOLD ) , fmin( fast_length( G1 - G0 ), COSTTHRESHOLDGRAD ), ALPHA ) );
				wsum += w[ ix ] * w[ iy ];
			}
		}
		Cout.x = c / wsum;
#else
		I0 = read_imagef( img0, sampler, coord  );
		G0 = read_imagef( grad0, sampler, coord );
		I1 = read_imagef( img1, sampler, coord2 );
		G1 = read_imagef( grad1, sampler, coord2 );
		Cout = ( float4 ) mix( fmin( dot( fabs( I1 - I0 ), dotmul ), COSTTHRESHOLD ) , fmin( dot( fabs( G1 - G0 ), dotmul ), COSTTHRESHOLDGRAD ), ALPHA );
//		Cout = ( float4 ) mix( fmin( fast_length( I1 - I0 ), COSTTHRESHOLD ) , fmin( fast_length( G1 - G0 ), COSTTHRESHOLDGRAD ), ALPHA );
#endif
	}

	write_imagef( costout, coord, Cout );
}
