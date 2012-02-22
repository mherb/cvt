/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/io/OpenNIManager.h>

namespace cvt {

	OpenNIManager& OpenNIManager::instance()
	{
		static OpenNIManager _instance;
		return _instance;
	}

	OpenNIManager::OpenNIManager()
	{
		updateDeviceList();
	}

	OpenNIManager::~OpenNIManager()
	{
	}

	void OpenNIManager::updateDeviceList()
	{
		xn::Context context;
		XnStatus status = context.Init();

		if( status != XN_STATUS_OK ){
			throw CVTException( "Error when initializing OpenNI context" );
		}

		xn::NodeInfoList devices;
		status = context.EnumerateProductionTrees( XN_NODE_TYPE_DEVICE, NULL, devices );

		xn::NodeInfoList::Iterator it= devices.Begin();
		xn::NodeInfoList::Iterator itEnd = devices.End();

		
		while( it!= itEnd ){
			xn::Device device;
			
			xn::NodeInfo nodeInfo( *it );
			status = context.CreateProductionTree( nodeInfo, device );

			DeviceInformation devInfo;
			nameAndSerialForDevice( devInfo.name, devInfo.serial, device );
			
			if( isNewDevice( devInfo.name, devInfo.serial ) ){
				size_t index = _deviceList.size();
				_deviceList.push_back( devInfo );
				fillCameraModes( index, device );
			}

			it++;
		}
	}

	void OpenNIManager::nameAndSerialForDevice( String& name, String& serial, xn::Device& device ) const
	{
		const size_t bufsize = 255;
		char buffer[ bufsize ];

		xn::DeviceIdentificationCapability idCap = device.GetIdentificationCap();
		XnStatus status = idCap.GetDeviceName( buffer, bufsize );
		if( status == XN_STATUS_OK ){
			name = buffer;
		} else {
			name = "UNKNOWN";
		}

		status = idCap.GetSerialNumber( buffer, bufsize );
		if( status == XN_STATUS_OK ){
			serial = buffer;
		} else {
			serial = "UNKNOWN";
		}
	}

	void OpenNIManager::fillCameraModes( size_t idx, xn::Device& device )
	{
		XnStatus status = XN_STATUS_OK;

		xn::Context			context;
		device.GetContext( context );
		
		// handle image generators
		xn::ImageGenerator	imgGen;
		if( createImageGeneratorForDevice( imgGen, idx, context ) ){
			XnUInt32 numModes = imgGen.GetSupportedMapOutputModesCount();
			XnMapOutputMode modes[ numModes ];

			status = imgGen.GetSupportedMapOutputModes( modes, numModes );
			if( status == XN_STATUS_OK ){
				std::vector<XnMapOutputMode> uniqueModes;
				getUniqueModeList( modes, numModes, uniqueModes );
				findSupportedModes( _deviceList[ idx ].rgbInfo , uniqueModes, imgGen );
			}
			
		}
		
		// check all supported image formats etc:
		xn::DepthGenerator	depthGen;
		if( createDepthGeneratorForDevice( depthGen, idx, context ) ){
			// find out about supported format etc.
			XnUInt32 numModes = depthGen.GetSupportedMapOutputModesCount();
			XnMapOutputMode modes[ numModes ];

			status = depthGen.GetSupportedMapOutputModes( modes, numModes );
			if( status == XN_STATUS_OK ){
				std::vector<XnMapOutputMode> uniqueModes;
				getUniqueModeList( modes, numModes, uniqueModes );

				std::cout << "Depth ModeS: " << std::endl;
				for( size_t i = 0; i < uniqueModes.size(); i++ ){
					std::cout << uniqueModes[ i ].nXRes << "x"
						      << uniqueModes[ i ].nYRes << "@"
						      << uniqueModes[ i ].nFPS << std::endl;
					_deviceList[ idx ].depthInfo.addMode( CameraMode( ( size_t )uniqueModes[ i ].nXRes, 
																	  ( size_t )uniqueModes[ i ].nYRes,
																      ( size_t )uniqueModes[ i ].nFPS, 
																	  IFormat::GRAY_UINT16 ) );
				}
			}
		}

		xn::IRGenerator	irGen;
		if( createIRGeneratorForDevice( irGen, idx, context ) ){
			// find out about supported format etc.
			// find out about supported format etc.
			XnUInt32 numModes = irGen.GetSupportedMapOutputModesCount();
			XnMapOutputMode modes[ numModes ];

			status = irGen.GetSupportedMapOutputModes( modes, numModes );
			if( status == XN_STATUS_OK ){
				std::vector<XnMapOutputMode> uniqueModes;
				getUniqueModeList( modes, numModes, uniqueModes );
				for( size_t i = 0; i < uniqueModes.size(); i++ ){
					std::cout << uniqueModes[ i ].nXRes << "x"
						      << uniqueModes[ i ].nYRes << "@"
						      << uniqueModes[ i ].nFPS << std::endl;
					_deviceList[ idx ].irInfo.addMode( CameraMode( uniqueModes[ i ].nXRes, uniqueModes[ i ].nYRes,
																   uniqueModes[ i ].nFPS, IFormat::GRAY_UINT16 ) );
				}
			}
		}
			
	}

			
	void OpenNIManager::getUniqueModeList( const XnMapOutputMode* modes, size_t num, std::vector<XnMapOutputMode> & unique )
	{
		for( size_t i = 0; i < num; i++ ){
			bool isNew = true;
			for( size_t x = 0; x < unique.size(); x++ ){
				if( unique[ x ].nXRes == modes[ i ].nXRes &&
				    unique[ x ].nYRes == modes[ i ].nYRes &&
				    unique[ x ].nFPS  == modes[ i ].nFPS ){
					isNew = false;
					break;
				}
			}
			if( isNew ){
				unique.push_back( modes[ i ] );
			}
		}
	}
	
	void OpenNIManager::findSupportedModes( CameraInfo& camInfo, const std::vector<XnMapOutputMode> & possibleModes, xn::MapGenerator & gen )
	{
		std::vector<OpenNIImageFormats> formatsToCheck;
		formatsToCheck.push_back( OPENNI_FORMAT_UNCOMPRESSED_YUV422 );
		formatsToCheck.push_back( OPENNI_FORMAT_UNCOMPRESSED_BAYER );
		formatsToCheck.push_back( OPENNI_FORMAT_UNCOMPRESSED_GRAY8 );

		IFormat format = IFormat::GRAY_UINT8;
		for( size_t i = 0; i < formatsToCheck.size(); i++ ){
			if( formatsToCheck[ i ] == OPENNI_FORMAT_UNCOMPRESSED_YUV422 )
				format = IFormat::UYVY_UINT8;
			else if( formatsToCheck[ i ] == OPENNI_FORMAT_UNCOMPRESSED_BAYER )
				format = IFormat::BAYER_GRBG_UINT8;
			else
				format = IFormat::GRAY_UINT8;

			if( gen.SetIntProperty( "InputFormat", formatsToCheck[ i ] ) == XN_STATUS_OK ){
				for( size_t m = 0; m < possibleModes.size(); m++ ){
					if( gen.SetMapOutputMode( possibleModes[ m ] ) == XN_STATUS_OK ){
						camInfo.addMode( CameraMode( possibleModes[ m ].nXRes,
													 possibleModes[ m ].nYRes,
													 possibleModes[ m ].nFPS,
													 format ) );
					}
				}
			}
		}

	}

	bool OpenNIManager::isNewDevice( const String& name, const String& serial ) const
	{
		for( size_t i = 0; i < _deviceList.size(); i++ ){
			if( _deviceList[ i ].name == name &&
				_deviceList[ i ].serial == serial )
				return false;
		}
		return true;
	}
    
    bool OpenNIManager::createDeviceForIdx( xn::Device& device, size_t idx, xn::Context& context )
    {
        xn::NodeInfoList devices;
		XnStatus status = context.EnumerateProductionTrees( XN_NODE_TYPE_DEVICE, NULL, devices );
        
        if( status != XN_STATUS_OK )
            throw CVTException( "Could not enumerate devices" );
        
		xn::NodeInfoList::Iterator it= devices.Begin();
		xn::NodeInfoList::Iterator itEnd = devices.End();
        
		while( it!= itEnd ){            
            xn::NodeInfo n( *it );
			if( nodeBelongsToDeviceIdx( idx, n, context ) ){
                status = context.CreateProductionTree( n, device );
                if( status != XN_STATUS_OK ){
                    throw CVTException( "Could not create production tree for device" ); 
                }
                return true;
            }

			it++;
		}
        return false;
    }


	bool OpenNIManager::createImageGeneratorForDevice( xn::ImageGenerator& generator, size_t idx, xn::Context& context )
	{
		XnStatus status = XN_STATUS_OK;

		xn::NodeInfoList nodeList;
		status = context.EnumerateProductionTrees( XN_NODE_TYPE_IMAGE, NULL, nodeList );

		xn::NodeInfoList::Iterator it = nodeList.Begin();
		xn::NodeInfoList::Iterator itEnd = nodeList.End();

		while( it != itEnd ){
			xn::NodeInfo n( *it );
			if( nodeBelongsToDeviceIdx( idx, n, context ) ){
				// create the image generator from this node and return
				status = context.CreateProductionTree( n, generator );
				if( status == XN_STATUS_OK )
					return true;
				else
					throw CVTException( "Could not create Image Generator for device" );
			}

			it++;
		}

		return false;
	}


	bool OpenNIManager::createDepthGeneratorForDevice( xn::DepthGenerator& generator, size_t idx, xn::Context& context )
	{
		XnStatus status = XN_STATUS_OK;

		xn::NodeInfoList nodeList;
		status = context.EnumerateProductionTrees( XN_NODE_TYPE_DEPTH, NULL, nodeList );

		xn::NodeInfoList::Iterator it = nodeList.Begin();
		xn::NodeInfoList::Iterator itEnd = nodeList.End();

		while( it != itEnd ){
			xn::NodeInfo n( *it );

			if( nodeBelongsToDeviceIdx( idx, n, context ) ){
				// create the image generator from this node and return
				status = context.CreateProductionTree( n, generator );
				if( status == XN_STATUS_OK )
					return true;
				else
					throw CVTException( "Could not create Image Generator for device" );
			}

			it++;
		}

		return false;
	}

	bool OpenNIManager::createIRGeneratorForDevice( xn::IRGenerator& generator, size_t idx, xn::Context& context )
	{
		XnStatus status = XN_STATUS_OK;

		xn::NodeInfoList nodeList;
		status = context.EnumerateProductionTrees( XN_NODE_TYPE_IR, NULL, nodeList );

		xn::NodeInfoList::Iterator it = nodeList.Begin();
		xn::NodeInfoList::Iterator itEnd = nodeList.End();

		while( it != itEnd ){
			xn::NodeInfo n( *it );

			if( nodeBelongsToDeviceIdx( idx, n, context ) ){
				// create the image generator from this node and return
				status = context.CreateProductionTree( n, generator );
				if( status == XN_STATUS_OK )
					return true;
				else
					throw CVTException( "Could not create Image Generator for device" );
			}

			it++;
		}

		return false;
	}
    
    void OpenNIManager::initializeImageGenerator( xn::ImageGenerator& generator, const CameraMode& mode )
    {
        OpenNIImageFormats format = OPENNI_FORMAT_UNCOMPRESSED_YUV422;
        if( mode.format == IFormat::UYVY_UINT8 ){
            format = OPENNI_FORMAT_UNCOMPRESSED_YUV422;
            generator.SetPixelFormat( XN_PIXEL_FORMAT_YUV422 );
        } else if( mode.format == IFormat::BAYER_GRBG_UINT8 ){
            format = OPENNI_FORMAT_UNCOMPRESSED_BAYER;
            generator.SetPixelFormat( XN_PIXEL_FORMAT_GRAYSCALE_8_BIT );
        } else {
            format = OPENNI_FORMAT_UNCOMPRESSED_GRAY8;
            generator.SetPixelFormat( XN_PIXEL_FORMAT_GRAYSCALE_8_BIT );
        }
        
        if( generator.SetIntProperty( "InputFormat", format ) != XN_STATUS_OK ){
            throw CVTException( "Cannot set requested InputFormat for device" );
        }
                
        XnMapOutputMode outputMode;
        outputMode.nXRes = mode.width;
        outputMode.nYRes = mode.height;
        outputMode.nFPS  = mode.fps;
    
        if( generator.SetMapOutputMode( outputMode ) != XN_STATUS_OK ){
            throw CVTException( "Could not set requested Map Outputmode" );
        }
    }
    
    void OpenNIManager::initializeDepthGenerator( xn::DepthGenerator& generator, const CameraMode& mode )
    {
        XnMapOutputMode outputMode;
        outputMode.nXRes = mode.width;
        outputMode.nYRes = mode.height;
        outputMode.nFPS  = mode.fps;
        
        if( generator.SetMapOutputMode( outputMode ) != XN_STATUS_OK ){
            throw CVTException( "Could not set requested Map Outputmode" );
        }   
    }
    
    void OpenNIManager::initializeIRGenerator( xn::IRGenerator& generator, const CameraMode& mode )
    {
        XnMapOutputMode outputMode;
        outputMode.nXRes = mode.width;
        outputMode.nYRes = mode.height;
        outputMode.nFPS  = mode.fps;
        
        if( generator.SetMapOutputMode( outputMode ) != XN_STATUS_OK ){
            throw CVTException( "Could not set requested Map Outputmode" );
        }
    }

	bool OpenNIManager::nodeBelongsToDeviceIdx( size_t idx, xn::NodeInfo& nodeInfo, xn::Context& context ) const 
	{
		xn::NodeInfoList& neededNodes = nodeInfo.GetNeededNodes();
		xn::NodeInfoList::Iterator it = neededNodes.Begin();
		xn::NodeInfoList::Iterator itEnd = neededNodes.End();

		const DeviceInformation& neededDevice = _deviceList[ idx ];

		String currName, currSerial;
		XnStatus status = XN_STATUS_OK;
		while( it != itEnd ){
			xn::NodeInfo n( *it );
			const XnProductionNodeDescription& desc = n.GetDescription();

			if( desc.Type == XN_NODE_TYPE_DEVICE ){
				xn::Device dev;
				status = context.CreateProductionTree( n, dev );
				// get the name
				nameAndSerialForDevice( currName, currSerial, dev );
				if( currName == neededDevice.name && currSerial == neededDevice.serial ){
					return true;
				}
			}

			it++;
		}

		return false;

	}

}
