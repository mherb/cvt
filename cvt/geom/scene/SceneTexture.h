#ifndef CVT_SCENETEXTURE_H
#define CVT_SCENETEXTURE_H

#include <cvt/geom/scene/SceneNode.h>

namespace cvt {
	class SceneTexture : public SceneNode {
		public:
							SceneTexture( const String& name );
							~SceneTexture();

			void			load( const String& path );
			const Image&	image();

		private:
			Image _image;
	};

	inline SceneTexture::SceneTexture( const String& name ) : SceneNode( name )
	{
	}

	inline SceneTexture::~SceneTexture()
	{
	}

	inline void SceneTexture::load( const String& path )
	{
		_image.load( path );
	}

	inline const Image& SceneTexture::image()
	{
		return _image;
	}


}

#endif
