/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2013, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_OPENNI2_CAMERA_H
#define CVT_OPENNI2_CAMERA_H

#include <cvt/io/Camera.h>
#include <OpenNI.h>

namespace cvt
{
    class OpenNI2Camera : public Camera
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

            OpenNI2Camera( size_t idx, const CameraMode& mode );
            ~OpenNI2Camera();

            void			startCapture();
            void			stopCapture();
            bool			nextFrame( size_t timeOut = 30 );
            const Image&	frame() const;
            const Image&	depth() const;

            size_t			width()  const { return _rgb.width();  }
            size_t			height() const { return _rgb.height(); }
            const IFormat&	format() const { return _rgb.format(); }
            const String&	identifier() const { return _identifier;}

            void            setSyncRGBDepth( bool val );

            // enable / disable registration of RGB to Depth frame
            void            setRegisterDepthToRGB( bool val );

            static size_t	count();
            static void		cameraInfo( size_t index, CameraInfo & info );

        private:
            CaptureMode			_captureMode;
            Image				_rgb;
            Image				_depth;
            String				_identifier;

    };
}

#endif
