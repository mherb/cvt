struct Mat3f {
	float3 m[ 3 ];
};

typedef struct Mat3f Mat3f;

inline float3 mat3f_transform( const Mat3f* mat, const float3 vec )
{
	return ( float3 ) ( dot( mat->m[ 0 ].xyz, vec ),
					   dot( mat->m[ 1 ].xyz, vec ),
					   dot( mat->m[ 2 ].xyz, vec ) );
}

inline float3 mat3f_transpose_transform( const Mat3f* mat, const float3 vec )
{
	return ( float3 ) ( dot( ( float3 ) ( mat->m[ 0 ].x, mat->m[ 1 ].x, mat->m[ 2 ].x ), vec ),
						dot( ( float3 ) ( mat->m[ 0 ].y, mat->m[ 1 ].y, mat->m[ 2 ].y ), vec ),
						dot( ( float3 ) ( mat->m[ 0 ].z, mat->m[ 1 ].z, mat->m[ 2 ].z ), vec ) );
}



inline float2 mat3f_transform_proj2( const Mat3f* mat, const float2 vec )
{
	float3 hvec = ( float3 ) ( vec.x, vec.y, 1.0f );
	float3 tmp = ( float3 ) ( dot( mat->m[ 0 ].xyz, hvec ),
						      dot( mat->m[ 1 ].xyz, hvec ),
				              dot( mat->m[ 2 ].xyz, hvec ) );
	return ( float2 ) ( tmp.x / tmp.z, tmp.y / tmp.z );
}

inline void mat3f_outer( Mat3f* dst, const float3 a, const float3 b )
{
	dst->m[ 0 ].xyz = a.x * b;
	dst->m[ 1 ].xyz = a.y * b;
	dst->m[ 2 ].xyz = a.z * b;
}

inline void mat3f_mul( Mat3f* dst, const Mat3f* a, const Mat3f* b )
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


inline void mat3f_mul_transpose( Mat3f* dst, const Mat3f* a, const Mat3f* b )
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

inline void mat3f_mul_scalar( Mat3f* dst, float s, const Mat3f* m )
{
	dst->m[ 0 ] = s * m->m[ 0 ];
	dst->m[ 1 ] = s * m->m[ 1 ];
	dst->m[ 2 ] = s * m->m[ 2 ];
}

inline void mat3f_add( Mat3f* dst, const Mat3f* a, const Mat3f* b )
{
	dst->m[ 0 ] = a->m[ 0 ] + b->m[ 0 ];
	dst->m[ 1 ] = a->m[ 1 ] + b->m[ 1 ];
	dst->m[ 2 ] = a->m[ 2 ] + b->m[ 2 ];
}

inline void mat3f_sub( Mat3f* dst, const Mat3f* a, const Mat3f* b )
{
	dst->m[ 0 ] = a->m[ 0 ] - b->m[ 0 ];
	dst->m[ 1 ] = a->m[ 1 ] - b->m[ 1 ];
	dst->m[ 2 ] = a->m[ 2 ] - b->m[ 2 ];
}

inline void mat3f_transpose( Mat3f* dst, const Mat3f* mat )
{
  dst->m[ 0 ] = ( float3 ) ( mat->m[ 0 ].x, mat->m[ 1 ].x, mat->m[ 2 ].x );
  dst->m[ 1 ] = ( float3 ) ( mat->m[ 0 ].y, mat->m[ 1 ].y, mat->m[ 2 ].y );
  dst->m[ 2 ] = ( float3 ) ( mat->m[ 0 ].z, mat->m[ 1 ].z, mat->m[ 2 ].z );
}

inline void mat3f_axisangle_rotation( Mat3f* mat, float3 rot )
{
        float rad = length( rot );
		float3 axis = normalize( rot );
        float x, y, z, c, s;
        float wx, wy, wz;
        float xx, yy, yz;
        float xy, xz, zz;
        float x2, y2, z2;

        c = cos( rad * 0.5f );
        s = sin( rad * 0.5f );

        x = axis.x * s;
        y = axis.y * s;
        z = axis.z * s;

        x2 = x + x;
        y2 = y + y;
        z2 = z + z;

        xx = x * x2;
        xy = x * y2;
        xz = x * z2;

        yy = y * y2;
        yz = y * z2;
        zz = z * z2;

        wx = c * x2;
        wy = c * y2;
        wz = c * z2;

        mat->m[ 0 ].x = 1.0f - ( yy + zz );
        mat->m[ 0 ].y = xy - wz;
        mat->m[ 0 ].z = xz + wy;

        mat->m[ 1 ].x = xy + wz;
        mat->m[ 1 ].y = 1.0f - ( xx + zz );
        mat->m[ 1 ].z = yz - wx;

        mat->m[ 2 ].x = xz - wy;
        mat->m[ 2 ].y = yz + wx;
        mat->m[ 2 ].z = 1.0f - ( xx + yy );
}

inline void mat3f_rotation( Mat3f* mat, float3 rot )
{
	float3 c;
	float3 s = sincos( rot, &c );

	mat->m[ 0 ].x =  c.y * c.z;
	mat->m[ 0 ].y = -c.y * s.z;
	mat->m[ 0 ].z =        s.y;

	mat->m[ 1 ].x = c.x * s.z + c.z * s.x * s.y;
	mat->m[ 1 ].y = c.x * c.z - s.x *  s.y * s.z;
	mat->m[ 1 ].z =               -c.y * s.x;

	mat->m[ 2 ].x =  s.x * s.z - c.x * c.z *  s.y;
	mat->m[ 2 ].y =  c.x * s.y * s.z + c.z * s.x;
	mat->m[ 2 ].z =                    c.x * c.y;
}

inline void mat3f_rotation_transpose( Mat3f* mat, float3 rot )
{
	float3 c;
	float3 s = sincos( rot, &c );

	mat->m[ 0 ].x =  c.y * c.z;
	mat->m[ 1 ].x = -c.y * s.z;
	mat->m[ 2 ].x =        s.y;

	mat->m[ 0 ].y = c.x * s.z + c.z * s.x *  s.y;
	mat->m[ 1 ].y = c.x * c.z - s.x *  s.y * s.z;
	mat->m[ 2 ].y =               -c.y * s.x;

	mat->m[ 0 ].z =       s.x * s.z - c.x * c.z *  s.y;
	mat->m[ 1 ].z =  c.x * s.y * s.z +     c.z * s.x;
	mat->m[ 2 ].z =                         c.x * c.y;
}

void mat3f_inverse( Mat3f* dst, Mat3f* src )
{
	float4 tmp, tmp2;
	float4 D;
	float det;

	tmp.x = src[ 1 ].y * src[ 2 ].z - src[ 2 ].y * src[ 1 ].z;
	tmp.y = src[ 2 ].x * src[ 1 ].z - src[ 1 ].x * src[ 2 ].z;
	tmp.z = src[ 1 ].x * src[ 2 ].y - src[ 2 ].x * src[ 1 ].y;
	tmp.w = 0.0f;

	det = dot( src[ 0 ], tmp );
	if( fabs( det ) < 1e-10f ) {
		dst[ 0 ] = ( float3 ) ( 0.0f );
		dst[ 1 ] = ( float3 ) ( 0.0f );
		dst[ 2 ] = ( float3 ) ( 0.0f );
	} else {
		D = ( float4 ) ( 1.0f / ( det ) );
		tmp = mat[ 0 ];
		dst[ 0 ] = cross( mat[ 1 ], mat[ 2 ] ) * D;
		tmp2 = mat[ 1 ];
		dst[ 1 ] = cross( mat[ 2 ], tmp ) * D;
		dst[ 2 ] = cross( tmp, tmp2 ) * D;
	}
}

float3 mat3_lusolve( Mat3f* mat, float3 b )
{
	// LU decomposition
	mat[ 1 ].x /= mat[ 0 ].x;
	mat[ 2 ].x /= mat[ 0 ].x;

	mat[ 1 ].yz -= mat[ 1 ].x * mat[ 0 ].yz;
	mat[ 2 ].yz -= mat[ 2 ].x * mat[ 0 ].yz;

	mat[ 2 ].y /= mat[ 1 ].y;
	mat[ 2 ].z -= mat[ 2 ].y * mat[ 1 ].z;

	// forward substitution
	b.y -= mat[ 1 ].x * b.x;
	b.z -= dot( mat[ 2 ].xy, b.xy );

	// backward substiution
	float4 x;
	x.z = b.z / mat[ 2 ].z;
	x.y = ( b.y - mat[ 1 ].z * x.z ) / mat[ 1 ].y;
	x.x = ( b.x - dot( mat[ 0 ].yz, x.yz ) ) / mat[ 0 ].x;
	x.w = 0.0f;
	return x;
}


