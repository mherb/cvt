__kernel void TSDFVolume_clear( global float2* cv, int width, int height, int depth  )
{
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int gz = get_global_id( 2 );
	global float2* cvptr = cv + gz * width * height + gy * width + gx;

	if( gx >= width || gy >= height || gz >= depth )
		return;

	*cvptr = ( float2 ) ( 0.0f, 0.0f );
}

__kernel void TSDFVolume_add( global float2* cv, unsigned width, unsigned int height, unsigned int depth, read_only image2d_t dmap, float dscale,
							  constant float4 K[ 3 ], constant float4 TG2CAM[ 3 ], float truncaction )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int gz = get_global_id( 2 );
	const int iwidth = get_image_width( dmap );
	const int iheight = get_image_height( dmap );
	global float2* cvptr = cv + gz * width * height + gy * width + gx;
	float4 gridpos = { gx, gy, gz, 1 }; // FIXME: 0.5 offset ?
	float4 gpos;
	float2 ipos;
	float  z, d, sdf, tsdf;

	if( gx >= width || gy >= height || gz >= depth )
		return;

	/* use the transformation to transform grid position to camera coordinates*/
	gpos = ( float4 ) ( dot( TG2CAM[ 0 ], gridpos ), dot( TG2CAM[ 1 ], gridpos ), dot( TG2CAM[ 2 ], gridpos ), 1 );

	/* project into camera */
	z = dot( K[ 2 ], gpos );
	ipos = ( float2 ) ( dot( K[ 0 ], gpos ), dot( K[ 1 ], gpos ) ) / z;

	if( ipos.x < iwidth && ipos.y < iheight && ipos.x > 0 && ipos.y > 0 ) { // FIXME: only test for d > 0 ?
		d = read_imagef( dmap, sampler, ipos ).x * dscale;
		if( d > 0 ) {
			sdf = gpos.z - d;
			tsdf = clamp( sdf / truncaction, -1.0f, 1.0f );
			/*		if( sdf > 0 )
					tsdf = min( 1.0f, sdf / truncaction );
					else
					tsdf = max( -1.0f, sdf / truncaction );*/
			float2 old = *cvptr;
			*cvptr = ( float2 ) ( ( old.x * old.y + tsdf * ( old.y + 1.0f )  ) / ( 2.0f * old.y + 1.0f ), old.y + 1.0f );
		}
	}
}
