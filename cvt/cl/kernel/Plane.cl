#import "Matrix3.cl"
#import "Matrix4.cl"

typedef float4 Planef

inline Planef plane_init( Planef p, float3 normal, float depth )
{
	p.xyz = normal;
	p.w   = depth;
}

inline Planef plane_transform( Planef p, const Mat4f* mat )
{
	Planef ret;
	// transform using mat^-T
	return ret;
}

inline void plane_homography( Mat3f* h, const Planef p, const Mat4f* Kdst, const Mat4f* Tdst, const Mat4f* Ksrc, const Mat4f* Tsrc )
{

}
