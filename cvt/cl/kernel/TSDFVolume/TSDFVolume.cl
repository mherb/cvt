#import "../Matrix4.cl"

const sampler_t SAMPLER_LIN = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;
const sampler_t SAMPLER_NN = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

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
							  const Mat4f TG2CAM, float truncaction )
{
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int gz = get_global_id( 2 );
	const int iwidth = get_image_width( dmap );
	const int iheight = get_image_height( dmap );
	global float2* cvptr = cv + ( gz * height + gy ) * width + gx;
	float4 gridpos = ( float4 ) ( gx, gy, gz, 1.0f );
	float4 gpos;
	float2 ipos;
	float d, sdf, tsdf, w;

	if( gx >= width || gy >= height || gz >= depth )
		return;

	/* use the transformation to transform grid position to camera coordinates*/
	gpos = mat4f_transform( &TG2CAM, gridpos );

	/* project into camera */
	ipos = gpos.xy / gpos.z; //+ ( float ) 0.5f;

	if( ipos.x < iwidth && ipos.y < iheight && ipos.x >= 0 && ipos.y >= 0 ) { // FIXME: only test for d > 0 ?
		d = read_imagef( dmap, SAMPLER_NN, ipos ).x * dscale;
		if( d > 0 && gpos.z > 0 ) {
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

static inline float TSDFVolume_trilinearValue( global float2* cv, int width, int height, int depth, float3 pos )
{
	pos = fmin( fmax( ( float3 ) 0.0f, pos ), ( float3 ) ( width - 2, height - 2, depth - 2 ) );

	float3 base  = floor( pos );
	float3 alpha = pos - base;
	int3   ibase = ( int3 )( base.x, base.y, base.z );

//	if( any( ibase ) || any( ibase - ( int3 ) ( width - 1, height - 1, depth - 1 ) ) )
//	   return 1e10f;

#define TSDFWEIGHT( _x, _y, _z ) ( *( cv + ( ( _z ) * height + ( _y ) ) * width + ( _x ) ) ).y

if( TSDFWEIGHT( ibase.x, ibase.y, ibase.z ) < 1.0f )
	return 1e10f;
if( TSDFWEIGHT( ibase.x, ibase.y, ibase.z + 1 ) < 1.0f )
	return 1e10f;
if( TSDFWEIGHT( ibase.x, ibase.y + 1, ibase.z ) < 1.0f )
	return 1e10f;
if( TSDFWEIGHT( ibase.x, ibase.y + 1, ibase.z + 1 ) < 1.0f )
	return 1e10f;
if( TSDFWEIGHT( ibase.x + 1, ibase.y, ibase.z ) < 1.0f )
	return 1e10f;
if( TSDFWEIGHT( ibase.x + 1, ibase.y, ibase.z + 1 ) < 1.0f )
	return 1e10f;
if( TSDFWEIGHT( ibase.x + 1, ibase.y + 1, ibase.z ) < 1.0f )
	return 1e10f;
if( TSDFWEIGHT( ibase.x + 1, ibase.y + 1, ibase.z + 1 ) < 1.0f )
	return 1e10f;



#define TSDFVALUE( _x, _y, _z ) ( *( cv + ( ( _z ) * height + ( _y ) ) * width + ( _x ) ) ).x
	float8 values;
	values.s0 = TSDFVALUE( ibase.x, ibase.y, ibase.z );
	values.s1 = TSDFVALUE( ibase.x, ibase.y, ibase.z + 1 );
	values.s2 = TSDFVALUE( ibase.x, ibase.y + 1, ibase.z );
	values.s3 = TSDFVALUE( ibase.x, ibase.y + 1, ibase.z + 1 );
	values.s4 = TSDFVALUE( ibase.x + 1, ibase.y, ibase.z );
	values.s5 = TSDFVALUE( ibase.x + 1, ibase.y, ibase.z + 1 );
	values.s6 = TSDFVALUE( ibase.x + 1, ibase.y + 1, ibase.z );
	values.s7 = TSDFVALUE( ibase.x + 1, ibase.y + 1, ibase.z + 1 );

	float4 interpx = mix( values.s0123, values.s4567, alpha.x );
	float2 interpy = mix( interpx.s01, interpx.s23, alpha.y );
	float  interpz = mix( interpy.s0, interpy.s1, alpha.z );

	return interpz;

#undef TSDFVALUE
#undef TSDFWEIGHT
}

__kernel void TSDFVolume_rayCastDepthmap( write_only image2d_t out, global float2* cv, int width, int height, int depth, const Mat4f TCAM2G, const Mat4f TG2CAM, float scale )
{
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int iwidth = get_image_width( out );
	const int iheight = get_image_height( out );

	if( gx >= iwidth || gy >= iheight )
		return;

	float3 rayOrigin = mat4f_transform( &TCAM2G, ( float4 ) ( 0.0f, 0.0f, 0.0f, 1.0f ) ).xyz;
	float3 raytmp    = mat4f_transform( &TCAM2G, ( float4 ) ( ( float ) gx, ( float ) gy, 1.0f, 1.0f ) ).xyz;
	float3 rayDir	 = normalize( raytmp - rayOrigin );

	float rayStart = TSDFVolume_rayStart( rayOrigin, rayDir, width, height, depth );
	float rayEnd   = TSDFVolume_rayEnd( rayOrigin, rayDir, width, height, depth );

	if( rayStart < rayEnd && isfinite( rayStart ) && isfinite( rayEnd ) && all(isfinite( rayDir )) ) {
		float val_prev, val, ret;
		float3 pos_prev, pos;
		float lambda_step = ( rayEnd - rayStart ) / 1000.0f;

		pos_prev = rayOrigin + rayDir * rayStart;
		val_prev = TSDFVolume_trilinearValue( cv, width, height, depth, pos_prev );
		ret = 0.0f;

		for( float lambda = rayStart + lambda_step; lambda <= rayEnd; lambda += lambda_step ) {
			pos = rayOrigin + rayDir * lambda;
			val = TSDFVolume_trilinearValue( cv, width, height, depth, pos );

			if ( val_prev < 0.0f && val > 0.0f) {
				break;
			}

			if ( val_prev > 0.0f && val < 0.0f) {
				float alpha = -val / ( val_prev - val );
				float3 gpos = ( mix( pos.z, pos_prev.z, alpha ) );
				ret = fmax( -( mat4f_transform( &TG2CAM, ( float4 ) ( gpos, 1.0f ) ).z ) * scale, 0.0f );
				break;
			}
			val_prev = val;
			pos_prev = pos;
		}
		write_imagef( out, ( int2 ) ( gx, gy ), ( float4 ) ret );
	} else {
		write_imagef( out, ( int2 ) ( gx, gy ), ( float4 ) 0.0f );
	}
}
