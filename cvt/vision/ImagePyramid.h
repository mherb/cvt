#ifndef CVT_IMAGE_PYRAMID_H
#define CVT_IMAGE_PYRAMID_H

namespace cvt
{
	class ImagePyramid
	{
		public:
			ImagePyramid( size_t octaves, float scaleFactor );

			Image&		 operator[]( size_t octave )	   { return _image[ octave ]; }
			const Image& operator[]( size_t octave ) const { return _image[ octave ]; }

			void update( const Image& img );
			void recompute();
			
			size_t octaves() const { return _image.size(); }
			float scaleFactor() const { return _scaleFactor; }

		private:
			std::vector<Image>	 _image;
			float				 _scaleFactor;
			IScaleFilterBilinear _filter;
	};

	inline ImagePyramid::ImagePyramid( size_t octaves, float scaleFactor ) :
		_scaleFactor( scaleFactor )
	{
		_image.resize( octaves );
	}

	inline void ImagePyramid::update( const Image& img )
	{
		_image[ 0 ] = img;
		recompute();
	}

	inline void ImagePyramid::recompute()
	{
		float w = _image[ 0 ].width();
		float h = _image[ 0 ].height();

		for( size_t i = 1; i < _image.size(); i++ ){
			w *= _scaleFactor;
			h *= _scaleFactor;
			_image[ i - 1].scale( _image[ i ], ( size_t )w, ( size_t )h, _filter );
		}
	}

}

#endif
