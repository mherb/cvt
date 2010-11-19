#include "DC1394Camera.h"
#include <cvt/util/Exception.h>

#include <sstream>

namespace cvt
{

	DC1394Camera::DC1394Camera( size_t camIndex, size_t width, size_t height, 
							    size_t fps, const IFormat & format ) :	
		_dmaBufNum( 10 ), _camIndex( camIndex ), _frame( width, height, format ), 
		_width( width ), _height( height ), _fps( fps ), _capturing( false ), 
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

		_framerate = DC1394_FRAMERATE_60;
		_mode = DC1394_VIDEO_MODE_640x480_MONO8;
		_frame.reallocate( width, height, IFormat::BAYER_RGGB_UINT8 );
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

		dc1394_video_set_iso_speed( _camera, _speed );
		dc1394_video_set_mode( _camera, _mode );
		dc1394_video_set_framerate( _camera, _framerate );
		dc1394_capture_setup( _camera, _dmaBufNum, DC1394_CAPTURE_FLAGS_DEFAULT );
		dc1394_video_set_transmission( _camera, DC1394_ON );
		enableWhiteBalanceAuto( false );
		enableShutterAuto( false );
		enableGainAuto( false );
		enableIrisAuto( false );
		setShutter( 1200 );

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


	void DC1394Camera::nextFrame( )
	{
		if( !_capturing )
			return;

		dc1394video_frame_t* frame;
		dc1394_capture_dequeue( _camera, DC1394_CAPTURE_POLICY_WAIT, &frame );
		size_t stride;
		uint8_t* dst = _frame.map( &stride );
		for( size_t i = 0; i < _height; i++ )
			memcpy( dst + i * stride, frame->image + i * _width * sizeof( uint8_t ), _width * sizeof( uint8_t ) );
		_frame.unmap( dst );

		/* FIXME: convert to image format ... */
		dc1394_capture_enqueue( _camera, frame );
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
		
		std::stringstream name;
		name << cam->vendor << " " << cam->model;
		info.setName( name.str() );

		dc1394_camera_free( cam );
		dc1394_camera_free_list( list );
		dc1394_free( handle );
	}
}
