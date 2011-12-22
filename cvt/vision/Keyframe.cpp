#include <cvt/vision/Keyframe.h>

namespace cvt
{
	Keyframe::Keyframe( const Eigen::Matrix4d& pose ) 
	{
		_pose.set( pose );
	}

	double Keyframe::distance( const Eigen::Matrix4d & transform ) const
	{
		const Eigen::Matrix4d & poseMat = _pose.transformation();
		return ( transform.block<3, 1>( 0, 3 ) - poseMat.block<3, 1>( 0, 3 ) ).squaredNorm();
	}
}
