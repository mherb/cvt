/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
