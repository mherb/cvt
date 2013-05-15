#import "../Matrix4.cl"

__kernel void TSDFVolume_clear( global float2* cv, int width, int height, int depth, float init  )
{
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int gz = get_global_id( 2 );
	global float2* cvptr = cv + gz * width * height + gy * width + gx;

	if( gx >= width || gy >= height || gz >= depth )
		return;

	*cvptr = ( float2 ) ( 1.0f, init );
}

__kernel void TSDFVolume_add( global float2* cv, int width, int height, int depth, read_only image2d_t dmap, float dscale,
							  Mat4f TG2CAM, float truncaction )
{
	const sampler_t SAMPLER_LIN = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;
	const sampler_t SAMPLER_NN = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int gz = get_global_id( 2 );
	const int iwidth = get_image_width( dmap );
	const int iheight = get_image_height( dmap );
	global float2* cvptr = cv + ( gz * height + gy ) * width + gx;
	float4 gridpos = ( float4 ) ( gx, gy, gz, 1.0f );
	float4 gpos;
	float2 ipos;
	float z, d, sdf, tsdf, w;

	if( gx >= width || gy >= height || gz >= depth )
		return;

	/* use the transformation to transform grid position to camera coordinates*/
	gpos = mat4f_transform( &TG2CAM, gridpos );
//	gpos = ( float4 ) ( dot( TG2CAM[ 0 ], gridpos ), dot( TG2CAM[ 1 ], gridpos ), dot( TG2CAM[ 2 ], gridpos ), 0.0f );

	/* project into camera */
	z = gpos.z;
//	ipos = gpos.xy / z;
	ipos = gpos.xy / z + ( float ) 0.5f;

	if( ipos.x < iwidth && ipos.y < iheight && ipos.x >= 0 && ipos.y >= 0 ) { // FIXME: only test for d > 0 ?
		d = read_imagef( dmap, SAMPLER_LIN, ipos ).x * dscale;
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

__kernel void TSDFVolume_sliceX( write_only image2d_t out, int v, global float2* cv, int width, int height, int depth  )
{
	const int gx = v;
	const int gy = get_global_id( 0 );
	const int gz = get_global_id( 1 );
	global float2* cvptr = cv + gz * width * height + gy * width + gx;

	if( gx >= width || gy >= height || gz >= depth )
		return;

	float4 val;
	val.xyz = clamp( ( ( *cvptr ).x ) + 0.5f, 0.0f, 1.0f );
	val.w   = 1.0f;
	write_imagef( out, ( int2 ) ( gy, gz ), val );
}

__kernel void TSDFVolume_sliceY( write_only image2d_t out, int v, global float2* cv, int width, int height, int depth  )
{
	const int gx = get_global_id( 0 );
	const int gy = v;
	const int gz = get_global_id( 1 );
	global float2* cvptr = cv + gz * width * height + gy * width + gx;

	if( gx >= width || gy >= height || gz >= depth )
		return;

	float4 val;
	val.xyz = clamp( ( ( *cvptr ).x ) + 0.5f, 0.0f, 1.0f );
	val.w   = 1.0f;
	write_imagef( out, ( int2 ) ( gx, gz ), val );
}

__kernel void TSDFVolume_sliceZ( write_only image2d_t out, int v, global float2* cv, int width, int height, int depth  )
{
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int gz = v;
	global float2* cvptr = cv + gz * width * height + gy * width + gx;

	if( gx >= width || gy >= height || gz >= depth )
		return;

	float4 val;
	val.xyz = clamp( ( ( *cvptr ).x ) + 0.5f, 0.0f, 1.0f );
	val.w   = 1.0f;
	write_imagef( out, ( int2 ) ( gx, gy ), val );
}


static inline float TSDFVolume_rayStart( const float3 origin, const float3 direction, int width, int height, int depth )
{
	float xmin = ( ( direction.x > 0.0f ? 0.0f : width )  - origin.x ) / direction.x;
	float ymin = ( ( direction.y > 0.0f ? 0.0f : height ) - origin.y ) / direction.y;
	float zmin = ( ( direction.z > 0.0f ? 0.0f : depth )  - origin.z ) / direction.z;

	return fmax( fmax( xmin, ymin ), zmin );
}

static inline float TSDFVolume_rayEnd( const float3 origin, const float3 direction, int width, int height, int depth )
{
	float xmin = ( ( direction.x > 0.0f ? width : 0.0f )  - origin.x ) / direction.x;
	float ymin = ( ( direction.y > 0.0f ? height : 0.0f ) - origin.y ) / direction.y;
	float zmin = ( ( direction.z > 0.0f ? depth : 0.0f )  - origin.z ) / direction.z;

	return fmin( fmin( xmin, ymin ), zmin );
}

__kernel void TSDFVolume_rayCast( write_only image2d_t out, global float2* cv, int width, int height, int depth )
{

}
