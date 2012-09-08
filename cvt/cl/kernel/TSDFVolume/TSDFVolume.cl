__kernel void TSDFVolume_clear( global float2* cv, int width, int height, int depth, float weight  )
{
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int gz = get_global_id( 2 );
	global float2* cvptr = cv + gz * width * height + gy * width + gx;

	if( gx >= width || gy >= height || gz >= depth )
		return;

	*cvptr = ( float2 ) ( 1.0f, weight );
}

__kernel void TSDFVolume_add( global float2* cv, int width, int height, int depth, read_only image2d_t dmap, float dscale,
							 constant float4 TG2CAM[ 3 ], float truncaction )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int gz = get_global_id( 2 );
	const int iwidth = get_image_width( dmap );
	const int iheight = get_image_height( dmap );
	global float2* cvptr = cv + gz * width * height + gy * width + gx;
	float4 gridpos = { gx, gy, gz, 1.0f }; // FIXME: 0.5 offset ?
	float4 gpos;
	float2 ipos;
	float z, d, sdf, tsdf, w;

	if( gx >= width || gy >= height || gz >= depth )
		return;

	/* use the transformation to transform grid position to camera coordinates*/
	gpos = ( float4 ) ( dot( TG2CAM[ 0 ], gridpos ), dot( TG2CAM[ 1 ], gridpos ), dot( TG2CAM[ 2 ], gridpos ), 0.0f );

	/* project into camera */
	z = gpos.z;
	ipos = gpos.xy / z;

	if( ipos.x < iwidth && ipos.y < iheight && ipos.x >= 0 && ipos.y >= 0 ) { // FIXME: only test for d > 0 ?
		d = read_imagef( dmap, sampler, ipos ).x * dscale;
		if( d > 0 && z > 0 ) {
			sdf = d - gpos.z;
			w = 1.0f;
			tsdf = sdf / truncaction; //clamp( sdf / truncaction, -1.0f, 1.0f );
			if( fabs( sdf ) <= truncaction ) {
					float2 old = *cvptr;
					*cvptr = ( float2 ) ( ( old.x * old.y + tsdf * w  ) / ( old.y + w ), old.y + w );
			}
		}
	}
}

__kernel void TSDFVolume_slice( write_only image2d_t out, int z, global float2* cv, int width, int height, int depth  )
{
	const int gy = get_global_id( 0 );
	const int gx = z;
	const int gz = get_global_id( 1 );
	global float2* cvptr = cv + gz * width * height + gy * width + gx;

	if( gx >= width || gy >= height || gz >= depth )
		return;

	write_imagef( out, ( int2 ) ( gy, gz ), ( float4 ) ( ( ( *cvptr ).x ) ) );
}
