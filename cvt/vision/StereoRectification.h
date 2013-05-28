/*
			CVT - Computer Vision Tools Library

	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
	PARTICULAR PURPOSE.
 */
#ifndef CVT_STEREO_RECTIFICATION_H
#define CVT_STEREO_RECTIFICATION_H

#include <cvt/vision/StereoCameraCalibration.h>

namespace cvt {

	class StereoRectification {
		public:
			StereoRectification( const CameraCalibration& left, const CameraCalibration& right );
			StereoRectification( const StereoRectification& other );

			void undistortLeft( Image& out, const Image& in ) const;
			void undistortRight( Image& out, const Image& in ) const;

		private:
			StereoCameraCalibration _rectifiedCalibration;
			Image	_leftWarp;
			Image	_rightWarp;
	};

}

#endif
