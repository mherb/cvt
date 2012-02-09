#ifndef CVT_OPENNI_CAMERA_H
#define CVT_OPENNI_CAMERA_H

#include <cvt/io/Camera.h>

#include <ni/XnCppWrapper.h>

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
				BAYER_UNCOMPRESSED	= 6,
			};

			OpenNICamera( size_t idx, const CameraMode& mode );
			~OpenNICamera();

            void			startCapture();
            void			stopCapture();
			void			nextFrame();
            const Image&	frame() const;
            const Image&	depth() const;

			size_t			width()  const { return _rgb.width();  }
			size_t			height() const { return _rgb.height(); }
			const IFormat&	format() const { return _rgb.format(); }
			const String&	identifier() const;

            static size_t	count();
			static void		cameraInfo( size_t index, CameraInfo & info );

		private:
			Image	_rgb;
			Image	_depth;
			String	_identifier;

			xn::Context			_context;
			xn::Device			_device;
			xn::DepthGenerator	_depthGen;
			xn::ImageGenerator	_imageGen;
			xn::IRGenerator		_irGen;

			void initDepthGenerator( size_t w, size_t h, size_t fps );
			void initImageGenerator( size_t w, size_t h, size_t fps );

			void copyDepth();
			void copyImage();

			static void deviceForId( xn::Device& device, xn::Context& context, size_t id );
	};
}

#endif
