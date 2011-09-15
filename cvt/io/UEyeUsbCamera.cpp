#include "UEyeUsbCamera.h"

#include <iostream>
#include <sstream>

namespace cvt
{
	UEyeUsbCamera::UEyeUsbCamera(  size_t camIndex, const CameraMode & mode ) :
		_camIndex( camIndex ), _camHandle( 0 ),
		_width( mode.width ), _height( mode.height ), _stride( _width ),
		_frame( _width, _height, mode.format )
	{
		this->open( mode );
		this->setPixelClock( 25 );
		this->setFramerate( 40.0 );
		this->setAutoShutter( false );
		this->setAutoSensorShutter( false );
		this->setExposureTime( 12 );
		this->setIdentifier();
	}

	UEyeUsbCamera::~UEyeUsbCamera()
	{
		if ( _camHandle != 0 ){
			this->stopCapture();
			this->freeMemories();
			is_ExitImageQueue( _camHandle );
			is_ExitCamera( _camHandle );
		}
	}

	const String & UEyeUsbCamera::identifier() const
	{
		return _identifier;
	}

	void UEyeUsbCamera::setIdentifier() 
	{
		CAMINFO camInfo;
		is_GetCameraInfo( _camHandle, &camInfo );
		_identifier = String( camInfo.SerNo );
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

		_camHandle = _camIndex | IS_USE_DEVICE_ID;

		if( !initCam() )
			throw CVTException( "Could not initialize camera" );

		// display initialization
		INT xPos;
		INT yPos;
		xPos = 0;
		yPos = 0;;

		double newFPS;

		is_SetAOI( _camHandle, IS_SET_IMAGE_AOI, &xPos, &yPos, &_width, &_height );
		is_GetImageMemPitch( _camHandle, &_stride );

		_frame.reallocate( _width, _height, _frame.format() );

		if( is_SetFrameRate( _camHandle, mode.fps, &newFPS ) == IS_NO_SUCCESS ){
			std::cout << "Could not set FrameRate" << std::endl;
		}

		this->initMemories( mode );
		if( is_InitImageQueue( _camHandle, 0 ) != IS_SUCCESS )
			std::cout << "COULD NOT INIT IMAGE QUEUE" << std::endl;

		is_SetColorMode( _camHandle, IS_CM_BAYER_RG8 );
	}

	void UEyeUsbCamera::initMemories( const CameraMode & mode )
	{
		for( size_t i = 0; i < _numImageBuffers; i++ ){
			is_AllocImageMem( _camHandle, _width, _height, mode.format.bpp * 8, (char**)&_buffers[ i ], &_bufferIds[ i ] );
			is_AddToSequence( _camHandle, (char*)_buffers[ i ], _bufferIds[ i ] );
		}

		if( is_SetImageMem( _camHandle, (char*)_buffers[ 0 ], _bufferIds[ 0 ] ) == IS_NO_SUCCESS )
		   throw CVTException( "Could not set current memory buffer" );
	}

	void UEyeUsbCamera::freeMemories()
	{
		for( size_t i = 0; i < _numImageBuffers; i++ ){
			is_FreeImageMem( _camHandle, (char*)_buffers[ i ], _bufferIds[ i ] );
		}
	}

	void UEyeUsbCamera::startCapture()
	{
		is_CaptureVideo ( _camHandle, IS_DONT_WAIT );

		is_InitEvent( _camHandle, NULL, IS_SET_EVENT_FRAME );
		is_EnableEvent( _camHandle, IS_SET_EVENT_FRAME );

		this->setWhiteBalanceOnce();
	}

	void UEyeUsbCamera::stopCapture()
	{
		is_DisableEvent( _camHandle, IS_SET_EVENT_FRAME );
		is_ExitEvent( _camHandle, IS_SET_EVENT_FRAME );

		if( is_StopLiveVideo( _camHandle, IS_WAIT ) == IS_NO_SUCCESS )
			throw CVTException( "Could not stop capture process" );
	}

	void UEyeUsbCamera::nextFrame()
	{
		uint8_t*	buffer;
		INT	bufferId = 0;

		INT ret = is_WaitEvent( _camHandle, IS_SET_EVENT_FRAME, 1000 );

		switch( ret ){
			case IS_SUCCESS:
				{
					// new frame available:
					if( is_GetActSeqBuf( _camHandle, &bufferId, NULL, ( char** )&buffer ) == IS_SUCCESS ){
						int bufSeqNum = bufNumForAddr( buffer );
						is_LockSeqBuf( _camHandle, bufSeqNum, ( char* )buffer );

						size_t stride;
						uint8_t * framePtr = _frame.map( &stride );

						if( _stride != ( int )stride )
							std::cout << "STRIDE ERROR" << std::endl;

						is_CopyImageMem( _camHandle, (char*)buffer, bufferId, (char*)framePtr );

						_frame.unmap( framePtr );

						if( is_UnlockSeqBuf( _camHandle, bufSeqNum, (char*)buffer ) == IS_NO_SUCCESS )
							std::cout << "UNLOCK FAILED" << std::endl;
					}
				}
				break;
			case IS_TIMED_OUT:
				std::cout << "Timeout in nextFrame()" << std::endl;
				break;
		}
	}

	size_t UEyeUsbCamera::count()
	{
		INT ret = 0;

		if( is_GetNumberOfCameras( &ret ) != IS_SUCCESS )
			std::cout << "FAILURE IN GET NUM CAMS" << std::endl;

		return (size_t)ret;
	}


	void UEyeUsbCamera::cameraInfo( size_t index, CameraInfo & info )
	{
		size_t camCount = ( ULONG )UEyeUsbCamera::count();
		if( index > camCount ){
			throw CVTException( "Index out of bounds" );
		}

		// get all cameras
		PUEYE_CAMERA_LIST camList = 0;
        camList = ( PUEYE_CAMERA_LIST )new char[ sizeof( DWORD ) + camCount * sizeof( UEYE_CAMERA_INFO ) ];
        camList->dwCount = camCount;

        // Get CameraList and store it ...
		if( is_GetCameraList( camList ) == IS_SUCCESS ){
			UEYE_CAMERA_INFO & cinfo = camList->uci[ index ];
			info.setName( cinfo.Model );
			info.setIndex( cinfo.dwDeviceID );
			info.setType( CAMERATYPE_UEYE );
			HIDS handle = ( cinfo.dwDeviceID );
			handle |= IS_USE_DEVICE_ID;
			delete[] camList;

			if( is_InitCamera( &handle, 0 ) == IS_CANT_OPEN_DEVICE )
				throw CVTException( "Cannot initialize camera" );

			SENSORINFO sensorInfo;
			memset( &sensorInfo, 0, sizeof( SENSORINFO ) );
			if( is_GetSensorInfo( handle, &sensorInfo ) == IS_NO_SUCCESS )
				throw CVTException( "Could not get image information" );

			INT pixClkMin = 0, pixClkMax = 0;
			if( is_GetPixelClockRange( handle, &pixClkMin, &pixClkMax ) == IS_NO_SUCCESS ){
				std::cout << "Could not get PixelClockRange" << std::endl;
			}
			if( is_SetPixelClock( handle, pixClkMax ) == IS_NO_SUCCESS ){
				std::cout << "Could not set PixelClock" << std::endl;
			}

			double min = 0, max = 0, interval = 0;
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
		} else {
			delete[] camList;
			throw CVTException( "Could not get camera list" );
		}
	}

    int	UEyeUsbCamera::bufNumForAddr( const uint8_t * buffAddr ) const
	{
		for( size_t i = 0; i < _numImageBuffers; i++ ){
			if( _buffers[ i ] == buffAddr )
				return ( int ) i+1;
		}

		return -1;
	}

}
