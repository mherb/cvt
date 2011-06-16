#include <cvt/vision/ORB.h>

namespace cvt {
	ORB::ORB( const Image& img, size_t octaves )
	{
		float scale = 1.0f;
		detect( img, scale );
		for( size_t i = 1; i < octaves; i++ ) {
			Image pyrimg;
			scale *=  scalefactor;
			img.resize( pyrimg, img.width() * scalefactor, img.height() * scalefactor );
			detect( pyrimg, scalefactor );
		}
	}

	void ORB::detect( const Image& img, float scalefactor )
	{
	}

	void ORB::centroidAngle( ORBFeature& feature, IntegralImage& iimg )
	{
	}

	void ORB::descriptor( ORBFeature& feature, IntegralImage& iimg )
	{
	}

}
