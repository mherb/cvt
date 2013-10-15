
#include <cvt/vision/StereoRectification.h>

namespace cvt {

	StereoRectification::StereoRectification( const CameraCalibration& left,
											  const CameraCalibration& right )
	{
		StereoCameraCalibration scalib( left, right );
		size_t w = 0;
		size_t h = 0;
		//_leftWarp.re
		scalib.undistortRectify( _rectifiedCalibration, _leftWarp, _rightWarp, w, h );
	}

	StereoRectification::StereoRectification( const StereoRectification& other ):
		_rectifiedCalibration( other._rectifiedCalibration ),
		_leftWarp( other._leftWarp ),
		_rightWarp( other._rightWarp )
	{}

	void StereoRectification::undistortLeft( Image& out, const Image& in ) const
	{
		IWarp::apply( out, in, _leftWarp );
	}

	void StereoRectification::undistortRight( Image& out, const Image& in ) const
	{
		IWarp::apply( out, in, _rightWarp );
	}

}
