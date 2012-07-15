/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include "DC1394Camera.h"
#include <cvt/util/Exception.h>

#include <cvt/util/String.h>

namespace cvt
{

	DC1394Camera::DC1394Camera( size_t camIndex, const CameraMode & mode ) :
		_dmaBufNum( 10 ), _camIndex( camIndex ), _frame( mode.width, mode.height, mode.format ),
		_width( mode.width ), _height( mode.height ), _fps( mode.fps ), _capturing( false ),
		_dcHandle( NULL ), _camera( NULL ),  _speed( DC1394_ISO_SPEED_400 )
	{
		_dcHandle = dc1394_new( );
		dc1394camera_list_t* list;

		dc1394_camera_enumerate( _dcHandle, &list );

		if( list->num == 0 ) {
			throw CVTException( "No Firewire cameras found" );
		}

		if( list->num <= _camIndex ) {
			throw CVTException(	"No camera with this index found" );
		}

		_camera = dc1394_camera_new( _dcHandle, list->ids[ _camIndex ].guid );
		dc1394_camera_free_list( list );

		if( !_camera )
			throw CVTException( "Could not open camera" );
		
		dcSettings( mode );
        
        _identifier.sprintf( "%llu", _camera->guid );
	}

	DC1394Camera::~DC1394Camera( )
	{
		close( );
		dc1394_camera_free( _camera );
		dc1394_free( _dcHandle );
	}

	void DC1394Camera::close( )
	{
		if( _capturing ) {
			stopCapture();
		}
	}

	void DC1394Camera::init( )
	{
		if( _capturing ) {
			stopCapture();
			startCapture();
		}
		reset();
	}

	void DC1394Camera::reset( )
	{
		dc1394_camera_reset( _camera );
	}


	void DC1394Camera::startCapture( )
	{
		this->init();
		
		if( _capturing )
			return;
		
		dc1394error_t error;
		
		error = dc1394_video_set_iso_speed( _camera, _speed );		
		if( error == DC1394_FAILURE ){
			throw CVTException( dc1394_error_get_string( error ) );
		}
		
		error = dc1394_video_set_mode( _camera, _mode );
		if( error == DC1394_FAILURE ){
			throw CVTException( dc1394_error_get_string( error ) );
		}
		
		if( dc1394_video_set_framerate( _camera, _framerate ) == DC1394_FAILURE ){
			throw CVTException( dc1394_error_get_string( error ) );
		}
		
		error = dc1394_capture_setup( _camera, _dmaBufNum, DC1394_CAPTURE_FLAGS_DEFAULT );
		if( error == DC1394_FAILURE ){
			throw CVTException( dc1394_error_get_string( error ) );
		}
		
		error = dc1394_video_set_transmission( _camera, DC1394_ON );
		if( error == DC1394_FAILURE ){
			throw CVTException( dc1394_error_get_string( error ) );
		}
		
//		enableWhiteBalanceAuto( false );
//		enableShutterAuto( false );
//		enableGainAuto( false );
//		enableIrisAuto( false );
//		setShutter( 300 );

		_capturing = true;
	}

	void DC1394Camera::stopCapture( )
	{
		if( !_capturing )
			return;
		dc1394_video_set_transmission( _camera, DC1394_OFF );
		dc1394_capture_stop( _camera );
		_capturing = false;
	}


	bool DC1394Camera::nextFrame( size_t /*timeout*/ )
	{
		if( !_capturing )
			return false;

		// TODO: howto use timeout with DC1394? This is blocking ATM!
		dc1394video_frame_t* frame;
		dc1394_capture_dequeue( _camera, DC1394_CAPTURE_POLICY_WAIT, &frame );
		
		size_t stride;
		uint8_t* dst = _frame.map( &stride );
		
		size_t bytesPerRow = ( frame->stride - frame->padding_bytes );
		
		for( size_t i = 0; i < _height; i++ )
			memcpy( dst + i * stride, frame->image + i * frame->stride, bytesPerRow );
		_frame.unmap( dst );

		/* FIXME: convert to image format ... */
		dc1394_capture_enqueue( _camera, frame );
		
		return true;
	}

	/**
	 * @brief Return a reference to the next image.
	 * @return reference to the image raw data within the dma ring buffer.
	 *
	 * Use reference to read the image data from the camera. You may also
	 * need to know the image size and format to avoid memory access violations.
	 */
	const Image& DC1394Camera::frame() const
	{
		if( !_capturing)
			throw CVTException( "Camera is not in capturing mode!" );
		return _frame;		
	}

	void DC1394Camera::enableWhiteBalanceAuto( bool enable )
	{
		dc1394feature_mode_t mode;

		if( enable ) {
			mode = DC1394_FEATURE_MODE_AUTO;
		} else   {
			mode = DC1394_FEATURE_MODE_MANUAL;
		}

		dc1394_feature_set_mode( _camera, DC1394_FEATURE_WHITE_BALANCE, mode );
	}

	void DC1394Camera::getWhiteBalance( unsigned int* ubValue, unsigned int* vrValue )
	{
		dc1394_feature_whitebalance_get_value( _camera, ubValue, vrValue );
	}

	void DC1394Camera::setWhiteBalance( unsigned int ubValue, unsigned int vrValue )
	{
		dc1394_feature_whitebalance_set_value( _camera, ubValue, vrValue );
	}

	void DC1394Camera::enableShutterAuto( bool enable )
	{
		dc1394feature_mode_t mode;

		if( enable ) {
			mode = DC1394_FEATURE_MODE_AUTO;
		} else   {
			mode = DC1394_FEATURE_MODE_MANUAL;
		}

		dc1394_feature_set_mode( _camera, DC1394_FEATURE_SHUTTER, mode );
	}

	void DC1394Camera::getShutter( unsigned int* value )
	{
		dc1394_feature_get_value( _camera, DC1394_FEATURE_SHUTTER, value );
	}

	void DC1394Camera::setShutter( unsigned int value )
	{
		dc1394_feature_set_value( _camera, DC1394_FEATURE_SHUTTER, value );
	}

	void DC1394Camera::enableGainAuto( bool enable )
	{
		dc1394feature_mode_t mode;

		if( enable ) {
			mode = DC1394_FEATURE_MODE_AUTO;
		} else   {
			mode = DC1394_FEATURE_MODE_MANUAL;
		}

		dc1394_feature_set_mode( _camera, DC1394_FEATURE_GAIN, mode );
	}

	void DC1394Camera::enableIrisAuto( bool enable )
	{
		dc1394feature_mode_t mode;

		if( enable ) {
			mode = DC1394_FEATURE_MODE_AUTO;
		} else   {
			mode = DC1394_FEATURE_MODE_MANUAL;
		}

		dc1394_feature_set_mode( _camera, DC1394_FEATURE_IRIS, mode );
	}
	
	void DC1394Camera::dcSettings( const CameraMode & mode )
	{
		// get equivalent dc video mode
		_mode = dcMode( mode );
		
		// set framerate equivalent:
		if( _fps ==240 )
			_framerate = DC1394_FRAMERATE_240;
		else if( _fps == 120 )
			_framerate = DC1394_FRAMERATE_120;
		else if( _fps == 60 )
			_framerate = DC1394_FRAMERATE_60;
		else if( _fps == 30 )
			_framerate = DC1394_FRAMERATE_30;
		else if( _fps == 15 )
			_framerate = DC1394_FRAMERATE_15;
		else if( _fps == 8 )
			_framerate = DC1394_FRAMERATE_7_5;
		else if( _fps == 4 )
			_framerate = DC1394_FRAMERATE_3_75;
		else 
			_framerate = DC1394_FRAMERATE_1_875;
		
		// TODO: check if device supports this mode?
	}
	
	dc1394video_mode_t DC1394Camera::dcMode( const CameraMode & mode ) {
		/* check fixed dc sizes */
		if( mode.width == 320 && mode.height == 240 ){
			if ( mode.format == IFormat::YUYV_UINT8 ) {
				return DC1394_VIDEO_MODE_320x240_YUV422;
			} else {
				throw CVTException( "No equivalent DC video mode for requested CameraMode" );
			}
		}
		
		if( mode.width == 640 && mode.height == 480 ){
			switch ( mode.format.formatID ) {
				case IFORMAT_UYVY_UINT8:		return DC1394_VIDEO_MODE_640x480_YUV422;
				case IFORMAT_BAYER_RGGB_UINT8:	return DC1394_VIDEO_MODE_640x480_MONO8;
				default:
					throw CVTException( "No equivalent dc1394 mode for given CameraMode" );
					break;
			}
		}
		
		if( mode.width == 800 && mode.height == 600 ){
			switch ( mode.format.formatID ) {
				case IFORMAT_YUYV_UINT8:		return DC1394_VIDEO_MODE_800x600_YUV422;
				case IFORMAT_BAYER_RGGB_UINT8:	return DC1394_VIDEO_MODE_800x600_MONO8;
				default:
					throw CVTException( "No equivalent dc1394 mode for given CameraMode" );
					break;
			}
		}

		if( mode.width == 1024 && mode.height == 768 ){
			switch ( mode.format.formatID ) {
				case IFORMAT_YUYV_UINT8:		return DC1394_VIDEO_MODE_1024x768_YUV422;
				case IFORMAT_BAYER_RGGB_UINT8:	return DC1394_VIDEO_MODE_1024x768_MONO8;
				default:
					throw CVTException( "No equivalent dc1394 mode for given CameraMode" );
					break;
			}
		}
		
		if( mode.width == 1280 && mode.height == 960 ){
			switch ( mode.format.formatID ) {
				case IFORMAT_YUYV_UINT8:		return DC1394_VIDEO_MODE_1280x960_YUV422;
				case IFORMAT_BAYER_RGGB_UINT8:	return DC1394_VIDEO_MODE_1280x960_MONO8;
				default:
					throw CVTException( "No equivalent dc1394 mode for given CameraMode" );
					break;
			}			
		}
		if( mode.width == 1600 && mode.height == 1200 ){
			switch ( mode.format.formatID ) {
				case IFORMAT_YUYV_UINT8:	return DC1394_VIDEO_MODE_1600x1200_YUV422;
				case IFORMAT_GRAY_UINT8:	return DC1394_VIDEO_MODE_1600x1200_MONO8;
				default:
					throw CVTException( "No equivalent dc1394 mode for given CameraMode" );
					break;
			}
		}
		
		throw CVTException( "No equivalent dc1394 mode for given CameraMode" );
	}

	size_t DC1394Camera::count()
	{
		size_t numCameras = 0;

		dc1394camera_list_t* list;
		dc1394_t* handle = dc1394_new( );
		dc1394_camera_enumerate( handle, &list );
		numCameras = list->num;
		dc1394_camera_free_list( list );
		dc1394_free( handle );
		return numCameras;
	}

	void DC1394Camera::cameraInfo( size_t index, CameraInfo & info )
	{
		info.setIndex( index );
		info.setType( CAMERATYPE_DC1394 );

		dc1394camera_list_t* list;
		dc1394_t* handle = dc1394_new( );
		dc1394_camera_enumerate( handle, &list );

		if( index > list->num )
			throw CVTException( "Camera index out of bounds" );

		dc1394camera_t * cam = 0;
		cam = dc1394_camera_new( handle, list->ids[ index ].guid );

		if( !cam )
			throw CVTException( "Could not create camera handle" );

		String name;
		name += cam->vendor;
		name += " ";
	    name += cam->model;
		info.setName( name );

		// get supported frame formats + speeds
		dc1394error_t error;
		dc1394video_modes_t videoModes;
		error = dc1394_video_get_supported_modes( cam, &videoModes );
		
		if( error == DC1394_FAILURE ){
			throw CVTException( "Could not query supported video modes from device" );
		}
		
		for( unsigned int i = 0; i < videoModes.num; i++ ){
			IFormat cvtFormat = IFormat::BGRA_UINT8;
			size_t width = 0, height = 0;
			switch ( videoModes.modes[ i ] ) {
				case DC1394_VIDEO_MODE_320x240_YUV422:
					cvtFormat = IFormat::UYVY_UINT8;
					width = 320; height = 240;
					break;
				case DC1394_VIDEO_MODE_640x480_YUV422:
					cvtFormat = IFormat::UYVY_UINT8;
					width = 640; height = 480;
					break;
				case DC1394_VIDEO_MODE_640x480_MONO8:
					cvtFormat = IFormat::BAYER_RGGB_UINT8;
					width = 640; height = 480;
					break;
				case DC1394_VIDEO_MODE_800x600_YUV422:
					cvtFormat = IFormat::UYVY_UINT8;
					width = 800; height = 600;
					break;
				case DC1394_VIDEO_MODE_800x600_MONO8:
					cvtFormat = IFormat::BAYER_RGGB_UINT8;
					width = 800; height = 600;
					break;
				case DC1394_VIDEO_MODE_1024x768_YUV422:
					cvtFormat = IFormat::UYVY_UINT8;
					width = 1024; height = 768;
					break;
				case DC1394_VIDEO_MODE_1024x768_MONO8:
					cvtFormat = IFormat::BAYER_RGGB_UINT8;
					width = 1024; height = 768;
					break;
				case DC1394_VIDEO_MODE_1280x960_YUV422:
					cvtFormat = IFormat::UYVY_UINT8;
					width = 1280; height = 960;
					break;
				case DC1394_VIDEO_MODE_1280x960_MONO8:
					cvtFormat = IFormat::BAYER_RGGB_UINT8;
					width = 1280; height = 960;
					break;
				case DC1394_VIDEO_MODE_1600x1200_YUV422:
					cvtFormat = IFormat::UYVY_UINT8;
					width = 1600; height = 1200;
					break;
				case DC1394_VIDEO_MODE_1600x1200_MONO8:
					cvtFormat = IFormat::BAYER_RGGB_UINT8;
					width = 1600; height = 1200;
					break;				
				case DC1394_VIDEO_MODE_1600x1200_MONO16:
				case DC1394_VIDEO_MODE_FORMAT7_0:
				case DC1394_VIDEO_MODE_FORMAT7_1:
				case DC1394_VIDEO_MODE_FORMAT7_2:
				case DC1394_VIDEO_MODE_FORMAT7_3:
				case DC1394_VIDEO_MODE_FORMAT7_4:
				case DC1394_VIDEO_MODE_FORMAT7_5:
				case DC1394_VIDEO_MODE_FORMAT7_6:
				case DC1394_VIDEO_MODE_FORMAT7_7:
				case DC1394_VIDEO_MODE_160x120_YUV444:
				case DC1394_VIDEO_MODE_640x480_YUV411:
				case DC1394_VIDEO_MODE_EXIF:
				default:
					continue;
					break;
			}
			
			dc1394framerates_t framerates;
			dc1394_video_get_supported_framerates( cam, videoModes.modes[ i ], &framerates );
			size_t fps = 0;
			for( size_t f = 0; f < framerates.num; f++ ){
				switch ( framerates.framerates[ f ]) {
					case DC1394_FRAMERATE_1_875:
						fps = 2;
						break;						
					case DC1394_FRAMERATE_3_75:
						fps = 4;
						break;
					case DC1394_FRAMERATE_7_5:
						fps = 8;
						break;
					case DC1394_FRAMERATE_15:
						fps = 15;
						break;
					case DC1394_FRAMERATE_30:
						fps = 30;
						break;
					case DC1394_FRAMERATE_60:
						fps = 60;
						break;
					case DC1394_FRAMERATE_120:
						fps = 120;
						break;
					case DC1394_FRAMERATE_240:
						fps = 240;
						break;
					default:
						break;
				}
				info.addMode( CameraMode( width, height, fps, cvtFormat ) );
			}
		}


		dc1394_camera_free( cam );
		dc1394_camera_free_list( list );
		dc1394_free( handle );
	}
}
