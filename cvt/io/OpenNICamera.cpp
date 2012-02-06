#include <cvt/io/OpenNICamera.h>

#include <cvt/io/OpenNIManager.h>

namespace cvt
{
	OpenNICamera::OpenNICamera()
	{
		XnStatus status = _context.Init();
		if( status != XN_STATUS_OK ){
			throw CVTException( "Could not initialize context" );
		}
		deviceForId( _device, _context, 0 );
	
		size_t w = 640; 	
		size_t h = 480; 	
		size_t fps = 30; 	
		initDepthGenerator( w, h, fps );
		initImageGenerator( w, h, fps );
	}

	OpenNICamera::~OpenNICamera()
	{
		stopCapture();
	}


	void OpenNICamera::startCapture()
	{
		XnStatus status = XN_STATUS_OK;
		status = _depthGen.StartGenerating();
		if( status != XN_STATUS_OK ){
			String msg;
			msg.sprintf( "Could not start generating Depth: %s", xnGetStatusString( status ) );
			throw CVTException( msg.c_str() );
		}
		status = _imageGen.StartGenerating();
		if( status != XN_STATUS_OK )
			throw CVTException( "Could not start Generating Image" );
	}

	void OpenNICamera::stopCapture()
	{
		XnStatus status = XN_STATUS_OK;
		status = _depthGen.StopGenerating();
		if( status != XN_STATUS_OK ){
			String msg;
			msg.sprintf( "Could not stop Generating Depth: %s", xnGetStatusString( status ) );
			throw CVTException( msg.c_str() );
		}

		status = _imageGen.StopGenerating();
		if( status != XN_STATUS_OK )
			throw CVTException( "Could not start Generating Image" );
	}

	void OpenNICamera::nextFrame()
	{
		XnStatus status = XN_STATUS_OK;

		status = _depthGen.WaitAndUpdateData();
		if( status != XN_STATUS_OK )
			throw CVTException( "Error in WaitAndUpdateData for depth" );
		copyDepth();

		status = _imageGen.WaitAndUpdateData();
		if( status != XN_STATUS_OK )
			throw CVTException( "Error in WaitAndUpdateData for image" );
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

	void OpenNICamera::initDepthGenerator( size_t w, size_t h, size_t fps )
	{
		XnStatus status = XN_STATUS_OK;
		
		_depth.reallocate( w, h, IFormat::GRAY_UINT16 );
		
		status = _depthGen.Create( _context );
		if( status != XN_STATUS_OK ){
			throw CVTException( "Could note create depth generator" );
		}

		XnMapOutputMode mode;
		mode.nXRes = w;
		mode.nYRes = h;
		mode.nFPS  = fps;
		status = _depthGen.SetMapOutputMode( mode );
		if( status != XN_STATUS_OK ){
			throw CVTException( "Could not set map outputmode for depth generator" );
		}
	}

	void OpenNICamera::initImageGenerator( size_t w, size_t h, size_t fps )
	{
		XnStatus status = XN_STATUS_OK;
		
		//_rgb.reallocate( w, h, IFormat::BAYER_RGGB_UINT8 );
		_rgb.reallocate( w, h, IFormat::UYVY_UINT8 );

		status = _imageGen.Create( _context );
		if( status != XN_STATUS_OK ){
			throw CVTException( "Could note create image generator" );
		}

		status = _imageGen.SetIntProperty( "InputFormat", 5 );
		if( status != XN_STATUS_OK )
			throw CVTException( "Could not set InputFormat" );

		//status = _imageGen.SetPixelFormat( XN_PIXEL_FORMAT_GRAYSCALE_8_BIT );
		status = _imageGen.SetPixelFormat( XN_PIXEL_FORMAT_YUV422 );
		if( status != XN_STATUS_OK )
			throw CVTException( "Could not set PixelFormat" );

		XnMapOutputMode mode;
		mode.nXRes = w;
		mode.nYRes = h;
		mode.nFPS  = fps;
		status = _imageGen.SetMapOutputMode( mode );
		if( status != XN_STATUS_OK ){
			throw CVTException( "Could not set map outputmode for image generator" );
		}
	}

	size_t OpenNICamera::count()
	{
		return OpenNIManager::instance().deviceCount();
	}
	
	void OpenNICamera::cameraInfo( size_t index, CameraInfo& info )
	{
		info = OpenNIManager::instance().cameraInfoForDevice( index );
	}

	static void enumerateImageModes( xn::Context & context )
	{
		XnStatus status = XN_STATUS_OK;

		xn::NodeInfoList nodeList;
		status = context.EnumerateProductionTrees( XN_NODE_TYPE_IMAGE, NULL, nodeList );

		xn::NodeInfoList::Iterator it = nodeList.Begin();
		xn::NodeInfoList::Iterator itEnd = nodeList.End();

		size_t n = 0;
		while( it != itEnd ){
			it++;
			n++;
		}
		std::cout << n << " possible Image nodes" << std::endl;
	}

	static void enumerateDepthNodes( xn::Context & context )
	{
		XnStatus status = XN_STATUS_OK;

		xn::NodeInfoList nodeList;
		status = context.EnumerateProductionTrees( XN_NODE_TYPE_DEPTH, NULL, nodeList );

		xn::NodeInfoList::Iterator it = nodeList.Begin();
		xn::NodeInfoList::Iterator itEnd = nodeList.End();

		size_t n = 0;
		while( it != itEnd ){
			it++;
			n++;
		}
		std::cout << n << " possible Depth nodes" << std::endl;
	}

	static void enumerateIrNodes( xn::Context & context )
	{
		XnStatus status = XN_STATUS_OK;

		xn::NodeInfoList nodeList;
		status = context.EnumerateProductionTrees( XN_NODE_TYPE_IR, NULL, nodeList );

		xn::NodeInfoList::Iterator it = nodeList.Begin();
		xn::NodeInfoList::Iterator itEnd = nodeList.End();

		size_t n = 0;
		while( it != itEnd ){
			it++;
			n++;
		}
		std::cout << n << " possible IR nodes" << std::endl;
	}


	void OpenNICamera::deviceForId( xn::Device& device, xn::Context& context, size_t idx )
	{
		xn::NodeInfoList devices;
		XnStatus status = context.EnumerateProductionTrees( XN_NODE_TYPE_DEVICE, NULL, devices );

		xn::NodeInfoList::Iterator it	 = devices.Begin();
		xn::NodeInfoList::Iterator itEnd = devices.End();

		size_t num = 0;
		while( num != idx ){
			num++;
			it++;
			if( it == itEnd )
				throw CVTException( "no camera with given id" );
		}

		xn::NodeInfo ninfo = *it;
		status = context.CreateProductionTree( ninfo, device );

		if( status != XN_STATUS_OK )
			throw CVTException( "Could not create production tree for device" );

	}
}
