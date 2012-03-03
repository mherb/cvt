/*
			CVT - Computer Vision Tools Library

	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
	PARTICULAR PURPOSE.
 */
#ifndef CVT_STEREOCAMERACALIBRATION
#define CVT_STEREOCAMERACALIBRATION

#include <cvt/vision/CameraCalibration.h>

namespace cvt {
	class StereoCameraCalibration
	{
		public:
			StereoCameraCalibration( const CameraCalibration& first, const CameraCalibration& second );
			StereoCameraCalibration( const CameraCalibration& first, const CameraCalibration& second, const Matrix4f& extrinsics );

			const CameraCalibration& firstCamera() const;
			const CameraCalibration& secondCamera() const;
			const Matrix4f&			 extrinsics() const;
			const void				 undistortRectify( StereoCameraCalibration& stereo ) const;

		private:
			CameraCalibration _first;
			CameraCalibration _second;
			Matrix4f		  _extrinsics;
	};

	inline StereoCameraCalibration::StereoCameraCalibration( const CameraCalibration& first, const CameraCalibration& second ) :
	   _first( first ),
	   _second( second )
	{
		_extrinsics =  _second.extrinsics() * _first.extrinsics().inverse();
	}

	inline StereoCameraCalibration::StereoCameraCalibration( const CameraCalibration& first, const CameraCalibration& second, const Matrix4f& extrinsics ) :
		_first( first ),
		_second( second ),
		_extrinsics( extrinsics )
	{
		_second.setExtrinsics( extrinsics * _first.extrinsics() );
	}

	inline const CameraCalibration& StereoCameraCalibration::firstCamera() const
	{
		return _first;
	}

	inline const CameraCalibration& StereoCameraCalibration::secondCamera() const
	{
		return _second;
	}

	inline const Matrix4f& StereoCameraCalibration::extrinsics() const
	{
		return _extrinsics;
	}

}

#endif
