/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/vision/features/FAST.h>
#include <cvt/gfx/IScaleFilter.h>

namespace cvt
{

	FAST::FAST( FASTSize size, uint8_t threshold, size_t border ) :
        _fastSize( size ),
		_threshold( threshold )
	{
		setBorder( border );
	}

	FAST::~FAST()
	{
	}

	void FAST::detect( FeatureSet& featureset, const Image& img )
	{
		if( img.format() != IFormat::GRAY_UINT8 )
			throw CVTException( "Input Image format must be GRAY_UINT8" );

		FeatureSetWrapper features( featureset );

		switch ( _fastSize ) {
			case SEGMENT_9:
				detect9( img, _threshold, features, _border );
				break;
			case SEGMENT_10:
				detect10( img, _threshold, features, _border );
				break;
			case SEGMENT_11:
				detect11( img, _threshold, features, _border );
				break;
			case SEGMENT_12:
				detect12( img, _threshold, features, _border );
				break;
			default:
				throw CVTException( "Unkown FAST size" );
				break;
		}
	}

	void FAST::detect( FeatureSet& featureset, const ImagePyramid& imgpyr )
	{
		if( imgpyr[ 0 ].format() != IFormat::GRAY_UINT8 )
			throw CVTException( "Input Image format must be GRAY_UINT8" );

		for( size_t coctave = 0; coctave < imgpyr.octaves(); coctave++ ) {
			float cscale = Math::pow( imgpyr.scaleFactor(), ( float ) -coctave );
			FeatureSetWrapper features( featureset, cscale, coctave );

			switch ( _fastSize ) {
				case SEGMENT_9:
					detect9( imgpyr[ coctave ], _threshold, features, _border );
					break;
				case SEGMENT_10:
					detect10( imgpyr[ coctave ], _threshold, features, _border );
					break;
				case SEGMENT_11:
					detect11( imgpyr[ coctave ], _threshold, features, _border );
					break;
				case SEGMENT_12:
					detect12( imgpyr[ coctave ], _threshold, features, _border );
					break;
				default:
					throw CVTException( "Unkown FAST size" );
					break;
			}
		}
	}

}
