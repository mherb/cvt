#include "muEyeUsbCamera.h"

#include <iostream>

namespace cvt
{

	UEyeUsbCamera::UEyeUsbCamera(  size_t camIndex, const CameraMode & mode ) :
		_camIndex( camIndex ), _camHandle( 0 ), _width( mode.width ), _height( mode.height ),
		_bufferID( -1 ), _bufferPtr( NULL ), _frame( _width, _height, mode.format )
	{
		this->open( mode );
	}

	UEyeUsbCamera::~UEyeUsbCamera()
	{
		if ( _camHandle != 0 )
		{
			//free old image mem.
			is_FreeImageMem( _camHandle, _bufferPtr, _bufferID );
			is_ExitCamera( _camHandle );
		}
	}

	void UEyeUsbCamera::setAutoGain( bool value )
	{
		double pVal1 = value?1:0;
		double pVal2 = 0;
		is_SetAutoParameter( _camHandle, IS_SET_ENABLE_AUTO_GAIN, &pVal1, &pVal2 );
	}

	void UEyeUsbCamera::setAutoShutter( bool value )
	{
		double pVal1 = value?1:0;
		double pVal2 = 0;
		is_SetAutoParameter( _camHandle, IS_SET_ENABLE_AUTO_SHUTTER, &pVal1, &pVal2 );
	}

	void UEyeUsbCamera::setAutoSensorShutter( bool value )
	{
		double pVal1 = value?1:0;
		double pVal2 = 0;
		is_SetAutoParameter( _camHandle, IS_SET_ENABLE_AUTO_SENSOR_SHUTTER, &pVal1, &pVal2 );
	}

	void UEyeUsbCamera::setAutoWhiteBalance( bool value )
	{
		double pVal1 = value?1:0;
		double pVal2 = 0;
		is_SetAutoParameter( _camHandle, IS_SET_ENABLE_AUTO_WHITEBALANCE, &pVal1, &pVal2 );
	}

	void UEyeUsbCamera::setMaxAutoShutter( double value )
	{
		double pVal1 = value;
		double pVal2 = 0;
		is_SetAutoParameter( _camHandle, IS_SET_AUTO_SHUTTER_MAX, &pVal1, &pVal2 );
	}

	void UEyeUsbCamera::setExposureTime( double value )
	{
		double pVal1 = value;
		is_SetExposureTime( _camHandle, pVal1, &pVal1 );
	}

	void UEyeUsbCamera::setPixelClock( unsigned int value )
	{
		is_SetPixelClock( _camHandle, value );
	}

	void UEyeUsbCamera::setHorizontalMirror( bool value )
	{
		is_SetRopEffect( _camHandle, IS_SET_ROP_MIRROR_UPDOWN, value?1:0, 0 );
	}

	void UEyeUsbCamera::setVerticalMirror( bool value )
	{
		is_SetRopEffect( _camHandle, IS_SET_ROP_MIRROR_LEFTRIGHT, value?1:0, 0 );
	}

	bool UEyeUsbCamera::initCam()
	{
		INT nRet = is_InitCamera( &_camHandle, NULL );
		if ( nRet == IS_STARTER_FW_UPLOAD_NEEDED ){
			// Time for the firmware upload = 25 seconds by default
			INT nUploadTime = 25000;
			is_GetDuration( _camHandle, IS_SE_STARTER_FW_UPLOAD, &nUploadTime );

			std::cout << "This camera requires a new firmware. The upload will take around" << nUploadTime / 1000 << " seconds. Please wait ..." <<std::endl;

			// specifying "IS_ALLOW_STARTER_FIRMWARE_UPLOAD"
			_camHandle = (HIDS)( ( (INT)_camHandle ) | IS_ALLOW_STARTER_FW_UPLOAD );
			nRet = is_InitCamera( &_camHandle, NULL );
		}

		if( nRet == IS_SUCCESS )
			return true;
		return false;
	}


	void UEyeUsbCamera::open( const CameraMode & mode )
	{
		if ( _camHandle != 0 ) {
			//free old image mem.
			is_FreeImageMem( _camHandle, _bufferPtr, _bufferID );
			is_ExitCamera( _camHandle );
		}

		_camHandle = _camIndex;

		if( !initCam() )
			throw CVTException( "Could not initialize camera" );

		// display initialization
		INT xPos;
		INT yPos;
		xPos = 0;
		yPos = 0;;

		double newFPS;

		//is_SetImageSize( _camHandle, _width, _height );
		is_SetAOI( _camHandle, IS_SET_IMAGE_AOI, &xPos, &yPos, &_width, &_height );

		_frame.reallocate( _width, _height, _frame.format() );

		if( is_SetFrameRate( _camHandle, mode.fps, &newFPS ) == IS_NO_SUCCESS ){
			std::cout << "Could not set FrameRate" << std::endl;
		}

		// FIXME: can't it be done differenty?
		is_AllocImageMem( _camHandle, _width, _height, mode.format.bpp * 8, &_bufferPtr, &_bufferID );
		is_SetImageMem( _camHandle, _bufferPtr, _bufferID );
		is_SetColorMode( _camHandle, IS_CM_BGR8_PACKED );
	}

	void UEyeUsbCamera::startCapture()
	{
		is_CaptureVideo ( _camHandle, IS_WAIT );
	}

	void UEyeUsbCamera::stopCapture()
	{
	}

	void UEyeUsbCamera::nextFrame()
	{
		// FIXME: find out how to do it right
		//is_WaitForNextImage( _camHandle )
		//is_CopyImageMem( _camHandle, _bufferPtr, _bufferID, img->imageData );
	}

	size_t UEyeUsbCamera::count()
	{
		INT ret;
		is_GetNumberOfCameras( &ret );

		return (size_t)ret;
	}


	void UEyeUsbCamera::cameraInfo( size_t index, CameraInfo & info )
	{
		if( index > UEyeUsbCamera::count() ){
			throw CVTException( "Index out of bounds" );
		}

		// retrieve camera information
		HIDS handle = index +1;// ids handle index starts at 1
		SENSORINFO sInfo;
		is_GetSensorInfo( handle, &sInfo );

		info.setName( std::string( sInfo.strSensorName ) );
		info.setIndex( index + 1 );
		info.setType( CAMERATYPE_UEYE );

	}

}
