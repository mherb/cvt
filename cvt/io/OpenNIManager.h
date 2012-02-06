#ifndef CVT_OPENNI_MANAGER_H
#define CVT_OPENNI_MANAGER_H

#include <cvt/util/String.h>
#include <cvt/io/CameraInfo.h>
#include <ni/XnCppWrapper.h>

namespace cvt {

	class OpenNIManager
	{
		public:

			struct DeviceInformation
			{
				String		name;
				String		serial;

				CameraInfo	rgbInfo;
				CameraInfo	depthInfo;
				CameraInfo	irInfo;
			};

			static OpenNIManager& instance();

			void updateDeviceList();

			size_t deviceCount() const { return _deviceList.size(); };
			const CameraInfo& cameraInfoForDevice( size_t idx ) const { return _deviceList[ idx ].rgbInfo; }
			

			/**
			 * @return: true if generator is valid, 
			 *			false if generator type is not supported 
			 */
			bool createImageGeneratorForDevice( xn::ImageGenerator& generator, size_t idx, xn::Context& context );
			bool createDepthGeneratorForDevice( xn::DepthGenerator& generator, size_t idx, xn::Context& context );
			bool createIRGeneratorForDevice( xn::IRGenerator& generator, size_t idx, xn::Context& context );

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

			void nameAndSerialForDevice( String& name, String& serial, xn::Device& device ) const;

			void findSupportedModes( size_t idx, const std::vector<XnMapOutputMode> & possibleModes, xn::ImageGenerator& gen );

			// device list:
			std::vector<DeviceInformation>	_deviceList;
	};

}

#endif
