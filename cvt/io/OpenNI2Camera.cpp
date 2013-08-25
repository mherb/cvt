#include <cvt/io/OpenNI2Camera.h>

namespace cvt {
    class Openni2Helper {
        public:
            static Openni2Helper& instance()
            {
                static Openni2Helper _instance;
                return _instance;
            }

            size_t numDevices()
            {
                return _camInfos.size();
            }

            const CameraInfo& deviceInfo( size_t idx )
            {
                return _camInfos[ idx ];
            }

        private:
            Openni2Helper()
            {
                openni::OpenNI::initialize();

                openni::OpenNI::enumerateDevices( &_deviceInfos );

                _camInfos.resize( _deviceInfos.getSize() );
                for( size_t i = 0; i < _deviceInfos.getSize(); ++i ){
                    CameraInfo& c = _camInfos[ i ];
                    const openni::DeviceInfo& devInfo = _deviceInfos[ i ];
                    c.setName(  devInfo.getName() );
                    c.setIndex( i );
                    c.setType( CAMERATYPE_OPENNI2 );
                    String id;
                    id.sprintf( "%s_%s", devInfo.getVendor(), devInfo.getUri() );
                    c.setIdentifier( id );

                    // add the supported modes
                    openni::Device d;
                    d.open( devInfo.getUri() );
                    const openni::SensorInfo* sinfo = d.getSensorInfo( openni::SENSOR_COLOR );
                    const openni::Array<openni::VideoMode>& vmodes = sinfo->getSupportedVideoModes();
                    for( size_t i = 0; i < vmodes.getSize(); ++i ){
                        const openni::VideoMode& vm = vmodes[ i ];
                        switch( vm.getPixelFormat() ){
                            case openni::PIXEL_FORMAT_RGB888:
                                c.addMode( CameraMode( vm.getResolutionX(), vm.getResolutionY(), vm.getFps(), IFormat::RGBA_UINT8 ) );
                                break;
                            case openni::PIXEL_FORMAT_YUV422:
                            case openni::PIXEL_FORMAT_YUYV:
                                c.addMode( CameraMode( vm.getResolutionX(), vm.getResolutionY(), vm.getFps(), IFormat::YUYV_UINT8 ) );
                                break;
                            case openni::PIXEL_FORMAT_GRAY8:
                                c.addMode( CameraMode( vm.getResolutionX(), vm.getResolutionY(), vm.getFps(), IFormat::GRAY_UINT8 ) );
                                break;
                            case openni::PIXEL_FORMAT_GRAY16:
                                c.addMode( CameraMode( vm.getResolutionX(), vm.getResolutionY(), vm.getFps(), IFormat::GRAY_UINT16 ) );
                                break;
                            default: continue; // just ignore
                        }
                    }
                    d.close();
                }
            }

            ~Openni2Helper()
            {
                openni::OpenNI::shutdown();
            }

            Openni2Helper( const Openni2Helper& );
            Openni2Helper& operator=( const Openni2Helper& );

            openni::Array<openni::DeviceInfo> _deviceInfos;
            std::vector<CameraInfo>           _camInfos;
    };

    OpenNI2Camera::OpenNI2Camera( size_t idx, const CameraMode& mode )
    {
    }
    
    OpenNI2Camera::~OpenNI2Camera()
    {
    }

    void OpenNI2Camera::startCapture()
    {
    }

    void OpenNI2Camera::stopCapture()
    {
    }

    bool OpenNI2Camera::nextFrame( size_t /*timeOut*/ )
    {
        return false;
    }
    
    const Image& OpenNI2Camera::frame() const
    {
        return _rgb;
    }
    
    const Image& OpenNI2Camera::depth() const
    {
        return _depth;
    }
    
    void OpenNI2Camera::setSyncRGBDepth( bool val )
    {
    }
    
    void OpenNI2Camera::setRegisterDepthToRGB( bool val )
    {
    }
    
    size_t OpenNI2Camera::count()
    {
        return Openni2Helper::instance().numDevices();
    }
    
    void OpenNI2Camera::cameraInfo( size_t index, CameraInfo & info )
    {
        info = Openni2Helper::instance().deviceInfo( index );
    }
}
