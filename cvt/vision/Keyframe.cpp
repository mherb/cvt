#include <cvt/vision/Keyframe.h>

namespace cvt
{
	Keyframe::Keyframe( const Image & img, const Eigen::Matrix4d& pose ) :
		_img( img ),
		_pose( pose )
	{
	}

	double Keyframe::distance( const Eigen::Matrix4d & transform ) const
	{
		return ( transform.block<3, 1>( 0, 3 ) - _pose.block<3, 1>( 0, 3 ) ).squaredNorm();
	}
}
