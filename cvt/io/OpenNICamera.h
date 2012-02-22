/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_OPENNI_CAMERA_H
#define CVT_OPENNI_CAMERA_H

#include <cvt/io/Camera.h>

#include <XnCppWrapper.h>

namespace cvt {

    class OpenNICamera : public Camera {
    public:

        enum InputFormat {
            BAYER_COMPRESSED = 0,
            UYVY_COMPRESSED = 1,
            JPEG = 2,
            UYVY_UNCOMPRESSED = 5,
            BAYER_UNCOMPRESSED = 6,
        };

        enum CaptureMode {
            DEPTH_ONLY,
            DEPTH_RGB,
            RGB_ONLY,
            // TODO: IR Modes. what is possible?
        };

        OpenNICamera(size_t idx, const CameraMode& mode);
        ~OpenNICamera();

        void startCapture();
        void stopCapture();
        void nextFrame();
        const Image& frame() const;
        const Image& depth() const;

        size_t width() const {
            return _rgb.width();
        }

        size_t height() const {
            return _rgb.height();
        }

        const IFormat& format() const {
            return _rgb.format();
        }
        const String& identifier() const;

        void setSyncRGBDepth(bool val);

        // enable / disable registration of RGB to Depth frame
        void setRegisterDepthToRGB(bool val);

        static size_t count();
        static void cameraInfo(size_t index, CameraInfo & info);

        void depthFocalLength() const;
        void setCaptureMode(CaptureMode mode);


    private:
        Image _rgb;
        Image _depth;
        String _identifier;

        xn::Context _context;
        xn::Device _device;
        xn::DepthGenerator      _depthGen;
        xn::ImageGenerator      _imageGen;
        xn::IRGenerator         _irGen;
        xn::ImageMetaData       _rgbData;
        xn::DepthMetaData       _depthData;

        CaptureMode _captureMode;

        void copyDepth();
        void copyImage();

        void startDepthCapture();
        void startImageCapture();
        void startIRCapture();
        void stopDepthCapture();
        void stopImageCapture();
        void stopIRCapture();

        static void deviceForId(xn::Device& device, xn::Context& context, size_t id);
    };
}

#endif
