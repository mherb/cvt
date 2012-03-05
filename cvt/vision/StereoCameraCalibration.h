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
#include <cvt/gfx/Image.h>

namespace cvt {
	class StereoCameraCalibration
	{
		public:
			StereoCameraCalibration( const CameraCalibration& first, const CameraCalibration& second );
			StereoCameraCalibration( const CameraCalibration& first, const CameraCalibration& second, const Matrix4f& extrinsics );

			void					 setFirstCamera( const CameraCalibration& firstcam );
			void					 setSecondCamera( const CameraCalibration& second );

			const CameraCalibration& firstCamera() const;
			const CameraCalibration& secondCamera() const;

			const Matrix4f&			 extrinsics() const;
			void					 setExtrinsics( const Matrix4f & extr );

			void					 undistortRectify( StereoCameraCalibration& stereo, Image& warpleft, Image& warpright,
													   size_t width, size_t height, bool verticalstereo = false ) const;

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

	inline void StereoCameraCalibration::setFirstCamera( const CameraCalibration& firstcam )
	{
		_first = firstcam;
	}

	inline void StereoCameraCalibration::setSecondCamera( const CameraCalibration& second )
	{
		_second = second;
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

    inline void StereoCameraCalibration::setExtrinsics( const Matrix4f & extr )
    {
        _extrinsics = extr;
    }

	inline void StereoCameraCalibration::undistortRectify( StereoCameraCalibration& stereo, Image& warpleft, Image& warpright, size_t width, size_t height, bool verticalstereo ) const
	{
		/*
			Algorithm from Matlab Camera Calibration Toolbox
		 */
		Vector3f axis, t, u;
		float fleft, fright, fnew;
		float angle;
		_extrinsics.toMatrix3().toAxisAngle( axis, angle );
		Matrix3f Rleft, Rright, R2;
		CameraCalibration cam0, cam1;

		/* align orientation of cameras, by rotating both cameras with half of the extrinsic rotation */
		Rleft.setRotation( axis,   0.5f * angle );
		Rright.setRotation( axis, -0.5f * angle );
		t = Rright * Vector3f( _extrinsics.col( 3 ) );

		/* find rotation so that the translation vector matches u */
		if( verticalstereo ) {
			u.set( 0.0f, 1.0f, 0.0f );
		} else {
			u.set( 1.0f, 0.0f, 0.0f );
		}
		if( u.dot( t ) < 0 )
			u = - u;

		axis  = t.cross( u );
		angle = acos( Math::abs( t.dot( u ) ) / ( t.length() * u.length() ) );
		R2.setRotation( axis, angle );

		/* apply the rotation to the left/right cameras*/
		Rright = R2 * Rright;
		Rleft  = R2 * Rleft;

		/* save the extrinsics of the new camera system*/
		Matrix4f extnew;
		extnew.setIdentity();
		cam0.setExtrinsics( extnew ); // cam0 is origin
		u = Rright * t;
		extnew[ 0 ][ 3 ] = u.x;
		extnew[ 1 ][ 3 ] = u.y;
		extnew[ 2 ][ 3 ] = u.z;
		stereo.setExtrinsics( extnew );
		cam1.setExtrinsics( extnew );

		/* the new focal lengths */
		fleft = _first.focalLength().y;
		if( _first.radialDistortion().x < 0 ) {
			fleft *= ( 1.0f + _first.focalLength().x * ( Math::sqr<float>( width ) + Math::sqr<float>( height ) ) / ( 4 * Math::sqr( _first.focalLength().y ) ) );
		}

		fright = _second.focalLength().y;
		if( _second.radialDistortion().x < 0 ) {
			fright *= ( 1.0f + _second.focalLength().x * ( Math::sqr<float>( width ) + Math::sqr<float>( height ) ) / ( 4 * Math::sqr( _second.focalLength().y ) ) );
		}

		fnew = Math::min( fleft, fright );

		/* the new principal points */

	}

}

#endif
