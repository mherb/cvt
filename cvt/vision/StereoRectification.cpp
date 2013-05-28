/*
			CVT - Computer Vision Tools Library

	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
	PARTICULAR PURPOSE.
 */

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
