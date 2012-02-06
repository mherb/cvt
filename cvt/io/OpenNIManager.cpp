#include <cvt/io/OpenNIManager.h>
#include <ni/XnCppWrapper.h>

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

			imgGen.SetPixelFormat( XN_PIXEL_FORMAT_GRAYSCALE_8_BIT );

			status = imgGen.GetSupportedMapOutputModes( modes, numModes );
			if( status == XN_STATUS_OK ){
				std::vector<XnMapOutputMode> uniqueModes;
				for( size_t i = 0; i < numModes; i++ ){
					bool isNew = true;
					for( size_t x = 0; x < uniqueModes.size(); x++ ){
						if( uniqueModes[ x ].nXRes == modes[ i ].nXRes &&
						    uniqueModes[ x ].nYRes == modes[ i ].nYRes &&
						    uniqueModes[ x ].nFPS  == modes[ i ].nFPS ){
							isNew = false;
							break;
						}
					}
					if( isNew ){
						uniqueModes.push_back( modes[ i ] );
					}
				}

				// now we have a unique set of modes
				// -> find out which work with which Image Formats
				findSupportedModes( idx, uniqueModes, imgGen );
			}
			
		}
		
		// check all supported image formats etc:
		xn::DepthGenerator	depthGen;
		if( createDepthGeneratorForDevice( depthGen, idx, context ) ){
			// find out about supported format etc.
		}

		xn::IRGenerator		irGen;
		if( createIRGeneratorForDevice( irGen, idx, context ) ){
			// find out about supported format etc.
		}
			
	}

	
	void OpenNIManager::findSupportedModes( size_t idx, const std::vector<XnMapOutputMode> & possibleModes, xn::ImageGenerator & gen )
	{
		CameraInfo& camInfo = _deviceList[ idx ].rgbInfo;

		std::vector<OpenNIImageFormats> formatsToCheck;
		formatsToCheck.push_back( OPENNI_FORMAT_UNCOMPRESSED_YUV422 );
		formatsToCheck.push_back( OPENNI_FORMAT_UNCOMPRESSED_BAYER );
		formatsToCheck.push_back( OPENNI_FORMAT_UNCOMPRESSED_GRAY8 );

		XnStatus status = XN_STATUS_OK;
		for( size_t i = 0; i < formatsToCheck.size(); i++ ){
			if( gen.SetIntProperty( "InputFormat", formatsToCheck[ i ] ) == XN_STATUS_OK ){
				for( size_t m = 0; m < possibleModes.size(); m++ ){
					if( gen.SetMapOutputMode( possibleModes[ m ] ) == XN_STATUS_OK ){
						std::cout << "Found Supported Mode: " << formatsToCheck[ i ] << ": " << 
							         possibleModes[ m ].nXRes << ", " << 
							         possibleModes[ m ].nYRes << ", " << 
							         possibleModes[ m ].nFPS << std::endl; 
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
