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

namespace cvt
{
    class OpenNICamera : public Camera
    {
        public:
            enum InputFormat
            {
                BAYER_COMPRESSED	= 0,
                UYVY_COMPRESSED		= 1,
                JPEG				= 2,
                UYVY_UNCOMPRESSED	= 5,
                BAYER_UNCOMPRESSED	= 6
            };

            enum CaptureMode
            {
                DEPTH_RGB,
                DEPTH_ONLY,
                RGB_ONLY
            };

            enum AntiFlickerMode
            {
                OFF   = XN_POWER_LINE_FREQUENCY_OFF,
                HZ_50 = XN_POWER_LINE_FREQUENCY_50_HZ,
                HZ_60 = XN_POWER_LINE_FREQUENCY_60_HZ
            };

            OpenNICamera( size_t idx, const CameraMode& mode );
            ~OpenNICamera();

            void			startCapture();
            void			stopCapture();
            bool			nextFrame( size_t timeOut = 30 );
            const Image&	frame() const;
            const Image&	depth() const;

            size_t			width()  const { return _rgb.width();  }
            size_t			height() const { return _rgb.height(); }
            const IFormat&	format() const { return _rgb.format(); }
            const String&	identifier() const;

            void            setSyncRGBDepth( bool val );

            // enable / disable registration of RGB to Depth frame
            void            setRegisterDepthToRGB( bool val );

            static size_t	count();
            static void		cameraInfo( size_t index, CameraInfo & info );

            void depthFocalLength() const;

            void setCaptureMode( CaptureMode mode );
            void startDepthCapture();
            void startImageCapture();
            void startIRCapture();
            void stopDepthCapture();
            void stopImageCapture();
            void stopIRCapture();

            void setAutoExposure( bool value );
            void setBacklightCompensation( bool value );
            void setAntiFlicker( AntiFlickerMode mode );
            void setGain();

            float           maxDepthRange() const;

        private:
            CaptureMode			_captureMode;
            Image				_rgb;
            Image				_depth;
            String				_identifier;

            xn::Context			_context;
            xn::Device			_device;

            xn::DepthGenerator	_depthGen;
            xn::ImageGenerator	_imageGen;
            xn::IRGenerator		_irGen;

            xn::DepthMetaData	_depthData;
            xn::ImageMetaData	_rgbData;

            void copyDepth();
            void copyImage();

            static void deviceForId( xn::Device& device, xn::Context& context, size_t id );
    };
}

#endif
