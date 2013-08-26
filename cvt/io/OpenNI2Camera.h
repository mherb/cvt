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
#include <cvt/io/RGBDInput.h>
#include <OpenNI.h>

namespace cvt
{
    class OpenNI2Camera : public Camera, RGBDInput
    {
        public:
            OpenNI2Camera( size_t idx, const CameraMode& mode );
            ~OpenNI2Camera();

            void			startCapture();
            void			stopCapture();
            bool			nextFrame( size_t timeOut = 30 );
            const Image&	frame() const { return _rgb; }
            const Image&	depth() const { return _depth; }

            size_t			width()  const { return _rgb.width();  }
            size_t			height() const { return _rgb.height(); }
            const IFormat&	format() const { return _rgb.format(); }
            const String&	identifier() const { return _identifier;}

            void            setSyncRGBDepth( bool val );

            // enable / disable registration of RGB to Depth frame
            void            setRegisterDepthToRGB( bool val );

            void            setAutoExposure( bool val );
            void            setAutoWhiteBalance( bool val );
            bool            autoExposure();
            bool            autoWhiteBalance();

            int             exposure();
            void            setExposure( int val );
            int             gain();
            void            setGain( int val );

            static size_t	count();
            static void		cameraInfo( size_t index, CameraInfo & info );

            /* RGBDInput interface */
            const Image&    rgb() const { return frame(); }
            void            next()      { nextFrame(); }
            double          stamp() const { return _rgbStamp; }
            double          depthStamp() const { return _depthStamp; }

        private:
            Image				_rgb;
            Image				_depth;
            String				_identifier;

            openni::Device      _device;
            openni::VideoStream _rgbStream;
            openni::VideoStream _depthStream;

            uint64_t            _rgbStamp;
            uint64_t            _depthStamp;

    };
}

#endif
