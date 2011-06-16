#include <cvt/vision/ORB.h>


namespace cvt {

	ORB::ORB( const Image& img, size_t octaves, float scalefactor )
	{
		float scale = 1.0f;
        IScaleFilterBilinear scaleFilter;
        detect( img, scale );
		for( size_t i = 1; i < octaves; i++ ) {
			Image pyrimg;
			scale *=  scalefactor;
			img.scale( pyrimg, ( size_t )( img.width() * scalefactor ), ( size_t )( img.height() * scalefactor ), scaleFilter );
			detect( pyrimg, scale );
		}
	}

	void ORB::detect( const Image& img, float scale )
	{
	}

	void ORB::centroidAngle( ORBFeature& feature, IntegralImage& iimg )
	{
	}

	void ORB::descriptor( ORBFeature& feature, IntegralImage& iimg )
	{
	}

#include "ORBPatterns.h"
}
