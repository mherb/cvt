#ifndef CVT_GLFORMAT_H
#define CVT_GLFORMAT_H

namespace cvt {

	class GLFormat {
		public:
			GLFormat();

			void defaultValues();

			void setRedSize( unsigned int size );
			unsigned int redSize();
			void setGreenSize( unsigned int size );
			unsigned int greenSize();
			void setBlueSize( unsigned int size );
			unsigned int blueSize();
			void setAlphaSize( unsigned int size );
			unsigned int alphaSize();
			void setDepthSize( unsigned int size );
			unsigned int depthSize();
			void setStencilSize( unsigned int size );
			unsigned int stencilSize();
			bool setDoubleBuffer( bool v );
			bool doubleBuffer();
			bool setDirect( bool v );
			bool direct();

		private:
			unsigned int _red, _green, _blue, _alpha, _depth, _stencil;
			bool _db, _direct;
	};

	GLFormat::GLFormat()
	{
		defaultValues();
	}

	inline void GLFormat::defaultValues()
	{
		_red = _green = _blue = _alpha = 8;
		_depth = 16;
		_stencil = 0;
		_db = true;
	}

	inline unsigned int GLFormat::redSize()
	{
		return _red;
	}

	inline unsigned int GLFormat::greenSize()
	{
		return _green;
	}

	inline unsigned int GLFormat::blueSize()
	{
		return _blue;
	}

	inline unsigned int GLFormat::alphaSize()
	{
		return _alpha;
	}

	inline unsigned int GLFormat::depthSize()
	{
		return _depth;
	}

	inline unsigned int GLFormat::stencilSize()
	{
		return _red;
	}

	inline bool GLFormat::doubleBuffer()
	{
		return _db;
	}

	inline bool GLFormat::direct()
	{
		return _direct;
	}

	inline void GLFormat::setRedSize( unsigned int v )
	{
		_red = v;
	}

	inline void GLFormat::setGreenSize( unsigned int v )
	{
		_green = v;
	}

	inline void GLFormat::setBlueSize( unsigned int v )
	{
		_blue = v;
	}

	inline void GLFormat::setAlphaSize( unsigned int v )
	{
		_alpha = v;
	}

	inline void GLFormat::setDepthSize( unsigned int v )
	{
		_depth = v;
	}

	inline void GLFormat::setStencilSize( unsigned int v )
	{
		_stencil = v;
	}

	inline void GLFormat::setDoubleBuffer( bool v )
	{
		_db = v;
	}

	inline void GLFormat::setDirect( bool v )
	{
		_direct = v;
	}
}

#endif
