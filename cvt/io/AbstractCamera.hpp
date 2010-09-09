#ifndef TOOLS_ABSTRACT_CAMERA
#define TOOLS_ABSTRACT_CAMERA

#include <cv.h>

namespace cvt {
    struct CameraParameters {
        CameraParameters():
		width(752),
		height(480),
		horizonticalMirror(false),
		verticalMirror(false),
		autoWhiteBalance(false),
		autoGain(false),
		autoShutter(false),
		autoSensorShutter(false),
		fps(60.0),
		pixelClock(20.0),
		maxAutoShutter(33.0),
		exposureTime(33.0)
        {
        }
		
        unsigned int width;
        unsigned int height;
		
        bool horizonticalMirror;
        bool verticalMirror;
        bool autoWhiteBalance;
        bool autoGain;
        bool autoShutter;
        bool autoSensorShutter;
		
        double fps;
        unsigned int pixelClock;
        double maxAutoShutter;
        double exposureTime;
    };
	
    class AbstractCamera
    {
	public:
		AbstractCamera(CameraParameters const& params = CameraParameters()):
		_width(params.width),
		_height(params.height)
        {
        }
		
		virtual ~AbstractCamera(){};
		
		virtual void init() = 0;
		
		virtual void captureNext() = 0;
		virtual void captureStart() = 0;
		virtual void captureStop() = 0;
		
		virtual IplImage const* image() = 0;
		
		virtual void setAutoGain(bool value)
		{std::cout << __FUNCTION__ << " not implemented" << std::endl;}
		
		virtual void setAutoShutter(bool value)
		{std::cout << __FUNCTION__ << " not implemented" << std::endl;}
		
		virtual void setAutoSensorShutter(bool value)
		{std::cout << __FUNCTION__ << " not implemented" << std::endl;}
		
		virtual void setAutoWhiteBalance(bool value)
		{std::cout << __FUNCTION__ << " not implemented" << std::endl;}
		
		virtual void setMaxAutoShutter(double value)
		{std::cout << __FUNCTION__ << " not implemented" << std::endl;}
		
		virtual void setExposureTime(double value)
		{std::cout << __FUNCTION__ << " not implemented" << std::endl;}
		
		virtual void setPixelClock(unsigned int value)
		{std::cout << __FUNCTION__ << " not implemented" << std::endl;}
		
		virtual void setHorizontalMirror(bool value)
		{std::cout << __FUNCTION__ << " not implemented" << std::endl;}
		
		virtual void setVerticalMirror(bool value)
		{std::cout << __FUNCTION__ << " not implemented" << std::endl;}
		
		virtual void setParameters(CameraParameters const& params)
		{std::cout << __FUNCTION__ << " not implemented" << std::endl;}
		
		unsigned int width(){ return _width; }
		unsigned int height(){ return _height; }
		
	protected:
		unsigned int _width;
		unsigned int _height;
    };
	
}

#endif
