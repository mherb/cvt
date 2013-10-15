#ifndef CVT_CGLCONTEXT_H
#define CVT_CGLCONTEXT_H

#include <cvt/gl/GLContext.h>

namespace cvt {
	class CGLContext : public GLContext
	{
		friend class ApplicationOSX;
		friend class WidgetImplWinGLOSX;
		public:
			CGLContext( const GLFormat& format, const CGLContext* share = NULL );
			~CGLContext();

			virtual void makeCurrent();
			virtual void swapBuffers();
			virtual void resetCurrent();

			virtual void shareCL( cl_context_properties* props, int size, int* retsize ) const;

			virtual GLPlatform platform() const { return GL_PLATFORM_OSX; }

			CGLContextObj cglContextObj() const { return _cglctx; }

		private:
			CGLContextObj _cglctx;
	};

	inline CGLContext::CGLContext( const GLFormat& format, const CGLContext* share ) : GLContext( format )
	{
		CGLPixelFormatObj pixfmt;
		GLint npix;

		// FIXME
		CGLPixelFormatAttribute attribs[ 20 ] = {
			kCGLPFAOpenGLProfile, (CGLPixelFormatAttribute)kCGLOGLPVersion_3_2_Core,
	//		kCGLPFAColorSize, ( CGLPixelFormatAttribute )  Math::max( _format.redSize(), Math::max( _format.greenSize(), _format.blueSize() ) ),
			kCGLPFAAlphaSize,	 ( CGLPixelFormatAttribute ) _format.alphaSize(),
			kCGLPFADepthSize,	 ( CGLPixelFormatAttribute ) _format.depthSize(),
			kCGLPFAStencilSize,  ( CGLPixelFormatAttribute ) _format.stencilSize(),
			kCGLPFADoubleBuffer,
			( CGLPixelFormatAttribute ) 0
		};

		if( _format.samples() != 0 ) {
			attribs[ 9 ] = kCGLPFASampleBuffers;
			attribs[ 10 ] = ( CGLPixelFormatAttribute ) 1;
			attribs[ 11 ] = kCGLPFASamples;
			attribs[ 12 ] = ( CGLPixelFormatAttribute ) _format.samples();
			attribs[ 13 ] = ( CGLPixelFormatAttribute ) 0;
		}

		if( CGLChoosePixelFormat( attribs, &pixfmt, &npix ) )
			throw CVTException( "Error: CGLContext creation failed - no Pixelformat found\n" );
		if( CGLCreateContext( pixfmt, share?share->_cglctx : NULL, &_cglctx ) )
			throw CVTException( "Error: CGLContext creation failed\n" );
	}

	inline CGLContext::~CGLContext()
	{
		resetCurrent();
		CGLReleaseContext( _cglctx );
	}

	inline void CGLContext::makeCurrent()
	{
		if( GLContext::currentContext() == this )
			return;
		if( CGLSetCurrentContext( _cglctx ) )
			std::cout << "Failed to set context" << std::endl;
		GLContext::setCurrent( this );
	}

	inline void CGLContext::swapBuffers()
	{
		CGLFlushDrawable( _cglctx );
	}

	inline void CGLContext::resetCurrent()
	{
		GLContext::setCurrent( NULL );
		CGLSetCurrentContext( NULL );
	}

	inline void CGLContext::shareCL( cl_context_properties* props, int size, int* retsize ) const
	{
		if( retsize ) {
			*retsize = 1;
			if( !props )
				return;
		}

		if( !props || size < 1 )
			throw CVTException( "Not enough memory for cl_context_properties!" );

		CGLShareGroupObj sharegroup = CGLGetShareGroup( _cglctx );

		*props++ = CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE;
	    *props++ = (cl_context_properties) sharegroup;
	}
}

#endif
