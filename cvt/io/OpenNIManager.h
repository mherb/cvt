/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_OPENNI_MANAGER_H
#define CVT_OPENNI_MANAGER_H

#include <cvt/util/String.h>
#include <cvt/io/CameraInfo.h>
#include <XnCppWrapper.h>

namespace cvt {

	class OpenNIManager
	{
		public:

			struct DeviceInformation
			{
				String		name;
				String		serial;
				String		vendorSpecific;

				CameraInfo	rgbInfo;
				CameraInfo	depthInfo;
				CameraInfo	irInfo;
			};

			static OpenNIManager& instance();

			void updateDeviceList();

			size_t deviceCount() const { return _deviceList.size(); }
			const CameraInfo& cameraInfoForDevice( size_t idx ) const { return _deviceList[ idx ].rgbInfo; }
			
			const DeviceInformation& deviceInfoForCam( size_t idx ) const 
			{ 
				return _deviceList[ idx ]; 
			}
        
            bool createDeviceForIdx( xn::Device& device, size_t idx, xn::Context& context );

			/**
			 * @return: true if generator is valid, 
			 *			false if generator type is not supported 
			 */
			bool createImageGeneratorForDevice( xn::ImageGenerator& generator, size_t idx, xn::Context& context );
			bool createDepthGeneratorForDevice( xn::DepthGenerator& generator, size_t idx, xn::Context& context );
			bool createIRGeneratorForDevice( xn::IRGenerator& generator, size_t idx, xn::Context& context );
        
            void initializeImageGenerator( xn::ImageGenerator& generator, const CameraMode& mode );
            void initializeDepthGenerator( xn::DepthGenerator& generator, const CameraMode& mode );
            void initializeIRGenerator( xn::IRGenerator& generator, const CameraMode& mode );
        

		private:
			enum OpenNIImageFormats
			{
				OPENNI_FORMAT_BAYER = 0,
				OPENNI_FORMAT_YUV422 = 1,
				OPENNI_FORMAT_JPEG = 2,
				OPENNI_FORMAT_JPEG_420 = 3,
				OPENNI_FORMAT_JPEG_MONO = 4,
				OPENNI_FORMAT_UNCOMPRESSED_YUV422 = 5,
				OPENNI_FORMAT_UNCOMPRESSED_BAYER = 6,
				OPENNI_FORMAT_UNCOMPRESSED_GRAY8 = 7
			};

			OpenNIManager();
			~OpenNIManager();

			/* declare but not define */
			OpenNIManager( const OpenNIManager& other );
			OpenNIManager& operator=( const OpenNIManager& other );
			
			bool isNewDevice( const String& name, const String& serial ) const;

			void fillBasicCameraInfo( CameraInfo& info, xn::Device& device );
			void fillCameraModes( size_t idx, xn::Device& device );

			bool nodeBelongsToDeviceIdx( size_t idx, xn::NodeInfo& nodeInfo, xn::Context& context ) const;

			void nameAndSerialForDevice( String& name, String& serial, String &vendorSpec, xn::Device &device ) const;

			void getUniqueModeList( const XnMapOutputMode* modes,  size_t num, std::vector<XnMapOutputMode> & unique );

			void findSupportedModes( CameraInfo& camInfo, const std::vector<XnMapOutputMode> & possibleModes, xn::MapGenerator& gen );

			// device list:
			std::vector<DeviceInformation>	_deviceList;
	};

	std::ostream& operator<<( std::ostream& out, const OpenNIManager::DeviceInformation& dInfo );

}

#endif
