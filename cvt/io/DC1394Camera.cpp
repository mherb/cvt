#include "DC1394Camera.h"
#include <cvt/util/Exception.h>

namespace cvt
{

	DC1394Camera::DC1394Camera(int camIndex, unsigned int width, unsigned int height, unsigned int fps, ImageChannelOrder order ) :	dmaBufNum( 5 ), mCamIndex( camIndex ), mFrame( NULL), mWidth( width ), mHeight( height ), mFps( fps ), mIorder( order ), capturing( false ), mDcHandle( NULL), mCamera( NULL ),  mSpeed( DC1394_ISO_SPEED_400 )
	{
		mDcHandle = dc1394_new( );
		dc1394camera_list_t* list;

		dc1394_camera_enumerate( mDcHandle, &list );

		if( list->num == 0 ) {
			throw CVTException( "No Firewire cameras found" );
		}

		if( list->num <= mCamIndex ) {
			throw CVTException(	"No camera with this index found" );
		}

		mCamera = dc1394_camera_new( mDcHandle, list->ids[ mCamIndex ].guid );
		dc1394_camera_free_list( list );

		if( !mCamera )
			throw CVTException( "Could not open camera" );

		mFramerate = DC1394_FRAMERATE_30;
		mMode = DC1394_VIDEO_MODE_640x480_MONO8;
		mFrame = new Image( 640, 480, CVT_GRAY, CVT_UBYTE );
	}

	DC1394Camera::~DC1394Camera( )
	{
		close( );
		dc1394_camera_free( mCamera );
		dc1394_free( mDcHandle );
		delete mFrame;
	}

	void DC1394Camera::open( )
	{
		enableWhiteBalanceAuto( true );
		enableShutterAuto( true );
		enableGainAuto( true );
		enableIrisAuto( true );
	}


	void DC1394Camera::close( )
	{
		if( capturing ) {
			captureStop( );
		}
	}

	void DC1394Camera::init( )
	{
		if( capturing ) {
			captureStop( );
			captureStart( );
		}
		reset();
	}

	void DC1394Camera::reset( )
	{
		dc1394_camera_reset( mCamera );
	}


	void DC1394Camera::captureStart( )
	{
		if( capturing )
			return;

		dc1394_video_set_iso_speed( mCamera, mSpeed );
		dc1394_video_set_mode( mCamera, mMode );
		dc1394_video_set_framerate( mCamera, mFramerate );
		dc1394_capture_setup( mCamera, dmaBufNum, DC1394_CAPTURE_FLAGS_DEFAULT );
		dc1394_video_set_transmission( mCamera, DC1394_ON );
		capturing = true;
	}

	void DC1394Camera::captureStop( )
	{
		if( !capturing )
			return;
		dc1394_video_set_transmission( mCamera, DC1394_OFF );
		dc1394_capture_stop( mCamera );
		capturing = false;
	}


	void DC1394Camera::captureNext( )
	{
		if( !capturing )
			return;

		dc1394video_frame_t* frame;
		dc1394_capture_dequeue( mCamera, DC1394_CAPTURE_POLICY_WAIT, &frame );
		memcpy( mFrame->data(), frame->image, mWidth * mHeight * sizeof( uint8_t ) );
		/* FIXME: convert to image format ... */
		dc1394_capture_enqueue( mCamera, frame );
	}

	/**
	 * @brief Return a pointer to the next image.
	 * @return Pointer to the image raw data within the dma ring buffer.
	 *
	 * Use this pointer to read the image data from the camera. You may also
	 * need to know the image size and format to avoid memory access violations.
	 */
	const Image* DC1394Camera::image( )
	{
		if( capturing)
			return mFrame;
		return NULL;
	}

	void DC1394Camera::enableWhiteBalanceAuto( bool enable )
	{
		dc1394feature_mode_t mode;

		if( enable ) {
			mode = DC1394_FEATURE_MODE_AUTO;
		} else   {
			mode = DC1394_FEATURE_MODE_MANUAL;
		}

		dc1394_feature_set_mode( mCamera, DC1394_FEATURE_WHITE_BALANCE, mode );
	}

	void DC1394Camera::getWhiteBalance( unsigned int* ubValue, unsigned int* vrValue )
	{
		dc1394_feature_whitebalance_get_value( mCamera, ubValue, vrValue );
	}

	void DC1394Camera::setWhiteBalance( unsigned int ubValue, unsigned int vrValue )
	{
		dc1394_feature_whitebalance_set_value( mCamera, ubValue, vrValue );
	}

	void DC1394Camera::enableShutterAuto( bool enable )
	{
		dc1394feature_mode_t mode;

		if( enable ) {
			mode = DC1394_FEATURE_MODE_AUTO;
		} else   {
			mode = DC1394_FEATURE_MODE_MANUAL;
		}

		dc1394_feature_set_mode( mCamera, DC1394_FEATURE_SHUTTER, mode );
	}

	void DC1394Camera::getShutter( unsigned int* value )
	{
		dc1394_feature_get_value( mCamera, DC1394_FEATURE_SHUTTER, value );
	}

	void DC1394Camera::setShutter( unsigned int value )
	{
		dc1394_feature_set_value( mCamera, DC1394_FEATURE_SHUTTER, value );
	}

	void DC1394Camera::enableGainAuto( bool enable )
	{
		dc1394feature_mode_t mode;

		if( enable ) {
			mode = DC1394_FEATURE_MODE_AUTO;
		} else   {
			mode = DC1394_FEATURE_MODE_MANUAL;
		}

		dc1394_feature_set_mode( mCamera, DC1394_FEATURE_GAIN, mode );
	}

	void DC1394Camera::enableIrisAuto( bool enable )
	{
		dc1394feature_mode_t mode;

		if( enable ) {
			mode = DC1394_FEATURE_MODE_AUTO;
		} else   {
			mode = DC1394_FEATURE_MODE_MANUAL;
		}

		dc1394_feature_set_mode( mCamera, DC1394_FEATURE_IRIS, mode );
	}

	int DC1394Camera::getNumberOfCameras( )
	{
		int numCameras = 0;

		dc1394camera_list_t* list;
		dc1394_t* handle = dc1394_new( );
		dc1394_camera_enumerate( handle, &list );
		numCameras = list->num;
		dc1394_camera_free_list( list );
		dc1394_free( handle );
		return numCameras;
	}
}
