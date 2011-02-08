#include "UEyeUsbCamera.h"

#include <iostream>
#include <sstream>

namespace cvt
{

	UEyeUsbCamera::UEyeUsbCamera(  size_t camIndex, const CameraMode & mode ) :
		_camIndex( camIndex ), _camHandle( 0 ),
		_width( mode.width ), _height( mode.height ),
		_frame( _width, _height, mode.format )
	{
		this->open( mode );
		this->setPixelClock( 40 );
		this->setFramerate( 80.0 );
		this->setAutoSensorShutter( false );
	}

	UEyeUsbCamera::~UEyeUsbCamera()
	{
		if ( _camHandle != 0 ){
			this->stopCapture();

			is_ExitImageQueue( _camHandle );
			this->freeMemories();
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

	void UEyeUsbCamera::setWhiteBalanceOnce()
	{
		double val = 1.0;
		is_SetAutoParameter( _camHandle, IS_SET_AUTO_WB_ONCE, &val , NULL );
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

	void UEyeUsbCamera::setFramerate( double fps )
	{
		double newFps;
		if( is_SetFrameRate( _camHandle, fps, &newFps ) == IS_NO_SUCCESS )
			std::cout << "New framerate: " << newFps << std::endl;
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
			this->freeMemories();
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

		is_SetAOI( _camHandle, IS_SET_IMAGE_AOI, &xPos, &yPos, &_width, &_height );

		_frame.reallocate( _width, _height, _frame.format() );

		if( is_SetFrameRate( _camHandle, mode.fps, &newFPS ) == IS_NO_SUCCESS ){
			std::cout << "Could not set FrameRate" << std::endl;
		}

		this->initMemories( mode );

		is_SetColorMode( _camHandle, IS_CM_BAYER_RG8 );
		is_InitImageQueue( _camHandle, 0 );
	}

	void UEyeUsbCamera::initMemories( const CameraMode & mode )
	{
		for( size_t i = 0; i < _numImageBuffers; i++ ){
			is_AllocImageMem( _camHandle, _width, _height, mode.format.bpp * 8, (char**)&_buffers[ i ], &_bufferIds[ i ] );
			is_AddToSequence( _camHandle, (char*)_buffers[ i ], _bufferIds[ i ] );
		}
		is_SetImageMem( _camHandle, (char*)_buffers[ 0 ], _bufferIds[ 0 ] );

	}

	void UEyeUsbCamera::freeMemories()
	{
		for( size_t i = 0; i < _numImageBuffers; i++ ){
			is_FreeImageMem( _camHandle, (char*)_buffers[ i ], _bufferIds[ i ] );
		}
	}

	void UEyeUsbCamera::startCapture()
	{
		is_CaptureVideo ( _camHandle, IS_WAIT );
		this->setWhiteBalanceOnce();
	}

	void UEyeUsbCamera::stopCapture()
	{
		if( is_StopLiveVideo( _camHandle, IS_WAIT ) == IS_NO_SUCCESS )
			throw CVTException( "Could not stop capture process" );
	}

	void UEyeUsbCamera::nextFrame()
	{
		uint8_t*	buffer;
		INT			bufferId;
		if( IS_TIMED_OUT == is_WaitForNextImage( _camHandle, 1000/*timeout in ms*/, (char**)&buffer, &bufferId ) ){
			throw CVTException( "Timeout when waiting for next image" );
		}

		size_t stride;
		uint8_t * framePtr = _frame.map( &stride );
		is_CopyImageMem( _camHandle, (char*)buffer, bufferId, (char*)framePtr );
		_frame.unmap( framePtr );

		is_UnlockSeqBuf( _camHandle, bufferId, (char*)buffer );
	}

	size_t UEyeUsbCamera::count()
	{
		INT ret = 0;

		if( is_GetNumberOfCameras( &ret ) == IS_NO_SUCCESS )
			throw CVTException( "Could not get number of cameras" );

		return (size_t)ret;
	}


	void UEyeUsbCamera::cameraInfo( size_t index, CameraInfo & info )
	{
		if( index > UEyeUsbCamera::count() ){
			throw CVTException( "Index out of bounds" );
		}

		UEYE_CAMERA_LIST * camList = new UEYE_CAMERA_LIST;
		camList->dwCount = 0;
		if( is_GetCameraList( camList ) == IS_NO_SUCCESS ){
			throw CVTException( "Could not get Camera list" );
		}
		DWORD numCams = camList->dwCount;
		delete camList;

		camList = (UEYE_CAMERA_LIST *)new char[ sizeof(DWORD) + numCams * sizeof( UEYE_CAMERA_INFO ) ];
		camList->dwCount = numCams;
		if( is_GetCameraList( camList ) == IS_NO_SUCCESS ){
			throw CVTException( "Could not get Camera list" );
		}

		info.setName( camList->uci[ index ].Model );
		info.setIndex( camList->uci[index].dwDeviceID );
		info.setType( CAMERATYPE_UEYE );

		// get frame information
		SENSORINFO sensorInfo;
		HIDS handle = (HIDS)( camList->uci[ index ].dwDeviceID | IS_USE_DEVICE_ID );

		if( is_InitCamera( &handle, 0 ) == IS_CANT_OPEN_DEVICE )
			throw CVTException( "Cannot initialize camera" );

		if( is_GetSensorInfo( handle, &sensorInfo ) == IS_NO_SUCCESS )
			throw CVTException( "Could not get image information" );

		INT pixClkMin, pixClkMax;
		if( is_GetPixelClockRange( handle, &pixClkMin, &pixClkMax ) == IS_NO_SUCCESS ){
			std::cout << "Could not get PixelClockRange" << std::endl;
		}

		if( is_SetPixelClock( handle, pixClkMax ) == IS_NO_SUCCESS ){
			std::cout << "Could not set PixelClock" << std::endl;
		}

		double min, max, interval;
		if( is_GetFrameTimeRange( handle, &min, &max, &interval ) == IS_NO_SUCCESS ){
			std::cout << "Could not get TimeRange" << std::endl;
		}

		size_t width = (size_t)sensorInfo.nMaxWidth;
		size_t height = (size_t)sensorInfo.nMaxHeight;
		size_t fps = (size_t)( 1.0 / min );
		if( sensorInfo.nColorMode == IS_COLORMODE_BAYER )
			info.addMode( CameraMode( width, height, fps, IFormat::BAYER_RGGB_UINT8 ) );
		else if( sensorInfo.nColorMode == IS_COLORMODE_MONOCHROME )
			info.addMode( CameraMode( width, height, fps, IFormat::GRAY_UINT8 ) );
		else
			throw CVTException( "Invalid color mode" );

		if( is_ExitCamera( handle ) == IS_CANT_CLOSE_DEVICE )
			throw CVTException( "Could not exit camera" );

		delete[] (char*)camList;
	}

}
