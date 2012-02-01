#include "UEyeUsbCamera.h"

#include <iostream>
#include <sstream>

namespace cvt
{
	UEyeUsbCamera::UEyeUsbCamera(  size_t camIndex, const CameraMode & mode ) :
		_camIndex( camIndex ), _camHandle( 0 ),
		_width( mode.width ), _height( mode.height ), _stride( _width ),
		_frame( _width, _height, mode.format ),
		_runMode( UEYE_MODE_FREERUN )
	{
		this->open( mode );
		this->setPixelClock( 35 );
		this->setFramerate( 40.0 );
		this->setExposureTime( 25 );
		this->setAutoShutter( true );
		//this->setAutoSensorShutter( true );
//		this->setAutoGain( true );
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
		int ret = is_SetAutoParameter( _camHandle, IS_SET_ENABLE_AUTO_SENSOR_SHUTTER, &pVal1, &pVal2 );
		if( ret != IS_SUCCESS ){
			std::cout << "Error when setting auto sensor shutter" << std::endl;
		}
	}

	void UEyeUsbCamera::setAutoWhiteBalance( bool value )
	{
		double val = value?1.0:0.0;
		int ret = is_SetAutoParameter( _camHandle, IS_SET_ENABLE_AUTO_WHITEBALANCE, &val, NULL );
		if( ret != IS_SUCCESS ){
			std::cout << "Error when setting auto white balance" << std::endl;
		}
	}	

	void UEyeUsbCamera::setWhiteBalanceOnce()
	{
		double val = 1.0;
		int ret = is_SetAutoParameter( _camHandle, IS_SET_AUTO_WB_ONCE, &val, NULL );
		if( ret != IS_SUCCESS ){
			std::cout << "Error when setting auto white balance once" << std::endl;
		}
		setAutoWhiteBalance( true );
	}

	void UEyeUsbCamera::setMaxAutoShutter( double value )
	{
		double pVal1 = value;
		double pVal2 = 0;
		is_SetAutoParameter( _camHandle, IS_SET_AUTO_SHUTTER_MAX, &pVal1, &pVal2 );
	}

	void UEyeUsbCamera::setExposureTime( double value )
	{
		double val;
		double min, max;
		exposureRange( min, max );
		
		if( value < min )
			val = min;
		else if ( value > max )
			val = max;
		val = value;
			
		is_Exposure( _camHandle, IS_EXPOSURE_CMD_SET_EXPOSURE, &val, sizeof( double ) );
	}

	void UEyeUsbCamera::exposureRange( double & min, double & max )
	{
		min = max = 0.0;
		is_Exposure( _camHandle, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MIN, &min, sizeof( double ) );
		is_Exposure( _camHandle, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MAX, &max, sizeof( double ) );
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

	void UEyeUsbCamera::getHardwareGains( int& master, int& red, int& green, int& blue )
	{
		master	= is_SetHardwareGain( _camHandle, IS_GET_MASTER_GAIN, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER );
		red		= is_SetHardwareGain( _camHandle, IS_GET_RED_GAIN, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER );
		green	= is_SetHardwareGain( _camHandle, IS_GET_GREEN_GAIN, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER );
		blue	= is_SetHardwareGain( _camHandle, IS_GET_BLUE_GAIN, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER );

		if( master	== IS_NO_SUCCESS ||
		    red		== IS_NO_SUCCESS ||
		    green   == IS_NO_SUCCESS ||
			blue	== IS_NO_SUCCESS )
			throw CVTException( "Error when getting hardware gains" );
	}

	void UEyeUsbCamera::setHardwareGains( int master, int red, int green, int blue )
	{
		int ret = is_SetHardwareGain( _camHandle, master, red, green, blue );
		if( ret != IS_SUCCESS )
			throw CVTException( "Error when setting hardware gains" );
	}

	void UEyeUsbCamera::setFramerate( double fps )
	{
		double newFps;
		if( is_SetFrameRate( _camHandle, fps, &newFps ) == IS_NO_SUCCESS ) {
			std::cout << "New framerate: " << newFps << std::endl;
		} else {
			setExposureTime( 15.0 );
		}
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

		IS_RECT aoiRect;
		aoiRect.s32X = 0;
		aoiRect.s32Y = 0;
		aoiRect.s32Width = _width;
		aoiRect.s32Height = _height;
		is_AOI( _camHandle, IS_AOI_IMAGE_SET_AOI, &aoiRect, sizeof( aoiRect ) );
		is_GetImageMemPitch( _camHandle, &_stride );

		_frame.reallocate( _width, _height, _frame.format() );

		double newFPS;
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
			_bufferIds[ 0 ] = 0;
			_buffers[ i ] = NULL;
			is_AllocImageMem( _camHandle, _width, _height, mode.format.bpp * 8, (char**)&_buffers[ i ], &_bufferIds[ i ] );
			is_AddToSequence( _camHandle, (char*)_buffers[ i ], _bufferIds[ i ] );
		}
	}

	void UEyeUsbCamera::freeMemories()
	{
		for( size_t i = 0; i < _numImageBuffers; i++ ){
			is_FreeImageMem( _camHandle, (char*)_buffers[ i ], _bufferIds[ i ] );
		}
	}

	void UEyeUsbCamera::startCapture()
	{
		bool val = ( _runMode == UEYE_MODE_FREERUN );
		setLiveMode( val );
		enableEvents();
	}

	void UEyeUsbCamera::stopCapture()
	{
		if( _runMode == UEYE_MODE_FREERUN ){
			disableFreerun();
		}

		disableEvents();
	}

	void UEyeUsbCamera::nextFrame()
	{
		uint8_t* curBuffer  = NULL;
		uint8_t* lastBuffer = NULL;
		INT	bufferId = 0;

		if( _runMode == UEYE_MODE_FREERUN ){
			INT ret = is_WaitEvent( _camHandle, IS_SET_EVENT_FRAME, 200 /* this is 5fps */ );
			switch( ret ){
				case IS_SUCCESS:
					{
						// new frame available:
						if( is_GetActSeqBuf( _camHandle, &bufferId, ( char** )&curBuffer, ( char** )&lastBuffer ) == IS_SUCCESS ){
							int bufSeqNum = bufNumForAddr( lastBuffer );
							is_LockSeqBuf( _camHandle, bufSeqNum, ( char* )lastBuffer );

							size_t stride;
							uint8_t * framePtr = _frame.map( &stride );
							is_CopyImageMem( _camHandle, (char*)lastBuffer, bufSeqNum, (char*)framePtr );
							
							if( is_UnlockSeqBuf( _camHandle, bufSeqNum, (char*)lastBuffer ) == IS_NO_SUCCESS ){
								std::cout << "UNLOCK FAILED" << std::endl;
							}
							
							_frame.unmap( framePtr );

						}
					}
					break;
				case IS_TIMED_OUT:
					std::cout << "Timeout in nextFrame()" << std::endl;
					break;
			}
		} else {
			is_SetImageMem( _camHandle, (char*)_buffers[ 0 ], _bufferIds[ 0 ]);
			if( is_FreezeVideo( _camHandle, IS_DONT_WAIT ) == IS_NO_SUCCESS ){
				std::cout << "ERROR IN IS_FREEZEVIDEO" << std::endl;
			}
		}
	}

	const Image & UEyeUsbCamera::frame() const
	{
		if( _runMode == UEYE_MODE_FREERUN ){
			return _frame;
		} else {
			INT ret = is_WaitEvent( _camHandle, IS_SET_EVENT_FRAME, 1000 );
			if ( ret == IS_SUCCESS ) {
				size_t stride;
				uint8_t * framePtr = _frame.map( &stride );
				is_CopyImageMem( _camHandle, (char*)_buffers[ 0 ], 0, (char*)framePtr );
				_frame.unmap( framePtr );
				return _frame;
			} else {
				std::cout << "timeout -> no new frame arrived" << std::endl;	
				return _frame;
			}	
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
			if( _buffers[ i ] == buffAddr ){
				return _bufferIds[ i ];
			}
		}

		return -1;
	}

	void UEyeUsbCamera::setLiveMode( bool val )
	{
		if( val ) {
			_runMode = UEYE_MODE_FREERUN;
			is_SetExternalTrigger( _camHandle, IS_SET_TRIGGER_OFF );
			enableFreerun();
		} else {
			if( _runMode == UEYE_MODE_FREERUN ){
				disableFreerun();
				enableTriggered();
			}
			is_SetExternalTrigger( _camHandle, IS_SET_TRIGGER_SOFTWARE );
			_runMode = UEYE_MODE_TRIGGERED;
		}
	}

	void UEyeUsbCamera::saveParameters( const String& filename ) const
	{
		int ret = is_SaveParameters( _camHandle, ( const IS_CHAR* )filename.c_str() );
		if( ret != IS_SUCCESS ){
			std::cout << "Could not save parameters to file" << std::endl;
		}
	}

	void UEyeUsbCamera::loadParameters( const String& filename )
	{
		int ret = is_LoadParameters( _camHandle, ( const IS_CHAR* )filename.c_str() );	
		if( ret != IS_SUCCESS )
			std::cout << "Error loading parameters from file" << std::endl;
	}

	void UEyeUsbCamera::enableFreerun()
	{
		is_CaptureVideo( _camHandle, IS_DONT_WAIT );
	}

	void UEyeUsbCamera::disableFreerun()
	{
		if( is_StopLiveVideo( _camHandle, IS_WAIT ) == IS_NO_SUCCESS )
			throw CVTException( "Could not stop capture process" );
	}

	void UEyeUsbCamera::enableTriggered()
	{
		/*
		UINT nMode = IO_FLASH_MODE_TRIGGER_HI_ACTIVE;
		is_IO( _camHandle, IS_IO_CMD_FLASH_SET_MODE, (void*)&nMode, sizeof(nMode) );
		*/
	}


	void UEyeUsbCamera::enableEvents()
	{
		is_EnableEvent( _camHandle, IS_SET_EVENT_FRAME );
	}

	void UEyeUsbCamera::disableEvents()
	{
		is_DisableEvent( _camHandle, IS_SET_EVENT_FRAME );
	}
}
