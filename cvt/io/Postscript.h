#ifndef CVT_POSTSCRIPT_H
#define CVT_POSTSCRIPT_H

#include <cvt/gfx/Drawable.h>
#include <cvt/io/GFXEnginePS.h>
#include <string>

namespace cvt {
	class Postscript : public Drawable
	{
		public:
			Postscript( std::string filename );
			virtual ~Postscript();
			virtual GFXEngine* gfxEngine();

		private:
			std::string _filename;
	};

	inline Postscript::Postscript( std::string filename ) : _filename( filename )
	{
	}

	inline Postscript::~Postscript()
	{
	}

	GFXEngine* Postscript::gfxEngine()
	{
		return new GFXEnginePS( _filename );
	}
}

#endif
