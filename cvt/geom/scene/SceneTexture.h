#ifndef CVT_SCENETEXTURE_H
#define CVT_SCENETEXTURE_H

#include <cvt/geom/scene/SceneNode.h>
#include <cvt/gfx/Image.h>

namespace cvt {
	class SceneTexture : public SceneNode {
		public:
							SceneTexture( const String& name );
							~SceneTexture();

			void			load( const String& path );
			const Image&	image() const;

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
		try {
			_image.load( path );
		} catch( Exception& e ) {
			// mark missing texture with PINK
			_image.reallocate( 16, 16, IFormat::RGBA_UINT8 );
			_image.fill( Color::PINK );
		}
	}

	inline const Image& SceneTexture::image() const
	{
		return _image;
	}


}

#endif
