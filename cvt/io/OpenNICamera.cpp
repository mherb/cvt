/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/io/OpenNICamera.h>

#include <cvt/io/OpenNIManager.h>

namespace cvt
{
	OpenNICamera::OpenNICamera( size_t idx, const CameraMode& mode ) :
        _captureMode( DEPTH_RGB )
	{
		XnStatus status = _context.Init();
		if( status != XN_STATUS_OK ){
			throw CVTException( "Could not initialize context" );
		}

		OpenNIManager& manager = OpenNIManager::instance();
        
        // create the device
        manager.createDeviceForIdx( _device, idx, _context );
        
        const OpenNIManager::DeviceInformation& devInfo = manager.deviceInfoForCam( idx );
        _identifier = devInfo.name;
        _identifier += "_";
        _identifier += devInfo.serial;
        
        if( !manager.createImageGeneratorForDevice( _imageGen, idx, _context ) ){
            // device does not support imageGen
            throw CVTException( "Device cannot generate rgb images" );
        }
        
        if( !manager.createDepthGeneratorForDevice( _depthGen, idx, _context ) ){
            // device does not support imageGen
            throw CVTException( "Device cannot generate rgb images" );
        }
        
        manager.initializeImageGenerator( _imageGen, mode );
        
        CameraMode m;
        m.width = 320; m.height = 240; m.fps = 60;
        
        _depth.reallocate( m.width, m.height, IFormat::GRAY_UINT16 );
        _rgb.reallocate( mode.width, mode.height, mode.format );

        manager.initializeDepthGenerator( _depthGen, m );
	}

	OpenNICamera::~OpenNICamera()
	{
		stopCapture();
	}


	void OpenNICamera::startCapture()
	{
		switch ( _captureMode ) {
            case DEPTH_ONLY:
                startDepthCapture();
                break;
            case DEPTH_RGB:
                startDepthCapture();
                startImageCapture();
                break;
            case RGB_ONLY:
                startImageCapture();
                break;
            default:
                throw CVTException( "Capture Mode not supported" );
                break;
        }
	}

	void OpenNICamera::stopCapture()
	{
        switch ( _captureMode ) {
            case DEPTH_ONLY:
                stopDepthCapture();
                break;
            case DEPTH_RGB:
                stopDepthCapture();
                stopImageCapture();
                break;
            case RGB_ONLY:
                stopImageCapture();
                break;
                
            default:
                throw CVTException( "Capture Mode not supported" );
                break;
        }
	}

	void OpenNICamera::nextFrame()
	{
		XnStatus status = XN_STATUS_OK;

        status = _context.WaitAndUpdateAll();
		if( status != XN_STATUS_OK )
			throw CVTException( "Error in WaitAndUpdateData for depth" );
        
		copyDepth();
		copyImage();
	}

	void OpenNICamera::copyDepth()
	{
		size_t stride;
		uint16_t* ptr = _depth.map<uint16_t>( &stride );
		const XnDepthPixel* dpixel = _depthGen.GetDepthMap();

		SIMD* simd = SIMD::instance();

		if( _depth.width() == stride ){
			simd->Memcpy( ( uint8_t* )ptr, ( const uint8_t* )dpixel, _depth.width() * _depth.height() * sizeof( uint16_t ) );
		} else {
			uint16_t* p = ptr;
			size_t h = _depth.height();
			size_t w = _depth.width();
			size_t bytesPerLine = w * sizeof( uint16_t );

			while( h-- ){
				simd->Memcpy( ( uint8_t* )p, ( const uint8_t* )dpixel, bytesPerLine );
				p += stride;
				dpixel += w; 
			}
		}

		_depth.unmap( ptr );
	}
    
    void OpenNICamera::setSyncRGBDepth( bool val )
    {
        xn::FrameSyncCapability syncCap = _imageGen.GetFrameSyncCap();
        if( syncCap.CanFrameSyncWith( _depthGen ) ){
            bool isSynced = syncCap.IsFrameSyncedWith( _depthGen );
            if( isSynced && !val ){
                syncCap.StopFrameSyncWith( _depthGen );
            } 
            if( !isSynced && val ){
                syncCap.FrameSyncWith( _depthGen );
            }
        }
    }
    
    void OpenNICamera::setRegisterDepthToRGB( bool val )
    {
        xn::AlternativeViewPointCapability cap = _depthGen.GetAlternativeViewPointCap();
        
        bool canRegisterToRGB = cap.IsViewPointSupported( _imageGen );
        if( canRegisterToRGB ){
            bool isRegistered = cap.IsViewPointAs( _imageGen );
            if( isRegistered && !val ){
                cap.ResetViewPoint();
            } else if( !isRegistered && val ) {
                cap.SetViewPoint( _imageGen );
            }
        }
    }
	
	void OpenNICamera::copyImage()
	{
		size_t stride;
		uint8_t* ptr = _rgb.map( &stride );

		const uint8_t* iptr = _imageGen.GetImageMap();
		size_t bytesPerLine = _rgb.width() * _imageGen.GetBytesPerPixel();
		
		SIMD* simd = SIMD::instance();

		size_t h = _rgb.height();

		if( bytesPerLine == stride ){
			simd->Memcpy( ptr, iptr, h * bytesPerLine );
		} else {
			uint8_t* p = ptr;
			while( h-- ){
				simd->Memcpy( p, iptr, bytesPerLine );
				p += stride;
				iptr += bytesPerLine; 
			}
		}

		_rgb.unmap( ptr );
	}
    
    void OpenNICamera::depthFocalLength() const
    {
        XnFieldOfView fov;
        XnStatus status = _depthGen.GetFieldOfView( fov );
        if( status != XN_STATUS_OK )
            throw CVTException( "Could not get FOV" );
        
        double fx =  _depth.width() / ( 2 * Math::tan( fov.fHFOV / 2.0 ) );
        double fy =  _depth.height() / ( 2 * Math::tan( fov.fVFOV / 2.0 ) );
        
        std::cout << fx << ", " << fy << std::endl;
        
        // pix size in mm
        XnDouble pixelSize;
        status = _depthGen.GetRealProperty( "ZPPS", pixelSize );
        if( status != XN_STATUS_OK ){
            String msg;
            msg.sprintf( "Could not get pixel size: %s", xnGetStatusString( status ) );
            throw CVTException( msg.c_str() );
        }
        
        // focal length in mm
        XnUInt64 depthFocalLengthFullRes;
        status = _depthGen.GetIntProperty( "ZPD", depthFocalLengthFullRes );
        if( status != XN_STATUS_OK ){
            String msg;
            msg.sprintf( "Could not get depth focal length: %s", xnGetStatusString( status ) );
            throw CVTException( msg.c_str() );
        }
      
        // baseline in cm
        XnDouble baseline;
        status = _depthGen.GetRealProperty( "LDDIS", baseline );
        if( status != XN_STATUS_OK ){
            String msg;
            msg.sprintf( "Could not get baseline: %s", xnGetStatusString( status ) );
            throw CVTException( msg.c_str() );
        }

        //focal length from mm -> pixels (valid for 1280x1024)
        std::cout << "Pixel Size: " << pixelSize << "mm" << std::endl;
        std::cout << "Focal len: " << depthFocalLengthFullRes / pixelSize << "px" << std::endl;
        std::cout << "Baseline: " << baseline << "cm" << std::endl;
    }
    
    void OpenNICamera::setCaptureMode( CaptureMode mode )
    {
        if( mode == _captureMode )
            return;
        stopCapture();
        _captureMode = mode;
        startCapture();
    }
    
			
	void OpenNICamera::startDepthCapture()
	{
		if( _depthGen.IsGenerating() )
            return;
        
		XnStatus status = XN_STATUS_OK;
		status = _depthGen.StartGenerating();
		if( status != XN_STATUS_OK ){
			String msg;
			msg.sprintf( "Could not start generating Depth: %s", xnGetStatusString( status ) );
			throw CVTException( msg.c_str() );
		}
	}

	void OpenNICamera::startImageCapture()
	{
        if( _imageGen.IsGenerating() )
            return;
        
		XnStatus status = XN_STATUS_OK;
		status = _imageGen.StartGenerating();
		if( status != XN_STATUS_OK ){
			String msg;
			msg.sprintf( "Could not start generating Image: %s", xnGetStatusString( status ) );
			throw CVTException( msg.c_str() );
		}
	}

	void OpenNICamera::startIRCapture()
	{
        if( _irGen.IsGenerating() )
            return;
        
		XnStatus status = XN_STATUS_OK;
		status = _irGen.StartGenerating();
		if( status != XN_STATUS_OK ){
			String msg;
			msg.sprintf( "Could not start generating IR Image: %s", xnGetStatusString( status ) );
			throw CVTException( msg.c_str() );
		}
	}

	void OpenNICamera::stopDepthCapture()
	{
        if( !_depthGen.IsGenerating() )
            return;
        
		XnStatus status = XN_STATUS_OK;
		status = _depthGen.StopGenerating();
		if( status != XN_STATUS_OK ){
			String msg;
			msg.sprintf( "Could not stop generating Depth: %s", xnGetStatusString( status ) );
			throw CVTException( msg.c_str() );
		}
	}

	void OpenNICamera::stopImageCapture()
	{
        if( !_imageGen.IsGenerating() )
            return;
        
		XnStatus status = XN_STATUS_OK;
		status = _imageGen.StopGenerating();
		if( status != XN_STATUS_OK ){
			String msg;
			msg.sprintf( "Could not stop generating Image: %s", xnGetStatusString( status ) );
			throw CVTException( msg.c_str() );
		}
	}

	void OpenNICamera::stopIRCapture()
	{
        if( !_irGen.IsGenerating() )
            return;
        
		XnStatus status = XN_STATUS_OK;
		status = _irGen.StopGenerating();
		if( status != XN_STATUS_OK ){
			String msg;
			msg.sprintf( "Could not stop generating IR Image: %s", xnGetStatusString( status ) );
			throw CVTException( msg.c_str() );
		}
	}

	const Image& OpenNICamera::frame() const 
	{
		return _rgb;
	}

	const Image& OpenNICamera::depth() const 
	{
		return _depth;
	}

	const String& OpenNICamera::identifier() const
	{
		return _identifier;
	}

	size_t OpenNICamera::count()
	{
		return OpenNIManager::instance().deviceCount();
	}
	
	void OpenNICamera::cameraInfo( size_t index, CameraInfo& info )
	{
		info = OpenNIManager::instance().cameraInfoForDevice( index );
	}

}
