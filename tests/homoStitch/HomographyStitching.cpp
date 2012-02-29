/* 
 * File:   HomographyStitching.cpp
 * Author: sebi
 * 
 * Created on February 27, 2012, 2:57 PM
 */

#include "HomographyStitching.h"
#include <cvt/math/sac/HomographySAC.h>
#include <cvt/math/sac/RANSAC.h>
#include <cvt/gfx/ifilter/ITransform.h>
#include <cvt/vision/FeatureMatcher.h>

namespace cvt {

    HomographyStitching::HomographyStitching() :
		_octaves( 3 ),
		_scale( 0.5f ),
		_fastCornerThreshold( 25 ),
		_maxNumFeatures( 0 ),
		_nonMaxSuppress( true ),
		_maxDescriptorDistance( 50 ),
		_stitched( 0 )
    {
		_stitched = new Image( 1280, 1024, IFormat::RGBA_UINT8 );
		_stitched->fill( Color::BLACK );

		_offsetTransform.setIdentity();
		_offsetTransform[ 0 ][ 0 ] = 0.5f;
		_offsetTransform[ 1 ][ 1 ] = 0.5f;
		_offsetTransform[ 0 ][ 2 ] = _stitched->width() / 4;
		_offsetTransform[ 1 ][ 2 ] = _stitched->height() / 4;
    }

    HomographyStitching::~HomographyStitching()
    {
		if( _stitched )
			delete _stitched;
    }

	void HomographyStitching::addImage( const Image& img )
	{

		// convert to grayscale: 
		Image gray;
		img.convert( gray, IFormat::GRAY_UINT8 );

		// calc the orb:
		ORB orb( gray, _octaves, _scale, _fastCornerThreshold, _maxNumFeatures, _nonMaxSuppress );

		if( _images.size() == 0 ){
			// add first image:
			_images.push_back( img );
			addFeatures( orb );
			_homographies.push_back( Matrix3f() );
			_homographies.back().setIdentity();

		//	_stitched->copyRect( _offsetTransform[ 0 ][ 2 ], _offsetTransform[ 1 ][ 2 ], img, img.rect() );
			ITransform::apply( *_stitched, _images.back(), _offsetTransform, _stitched->width(), _stitched->height() );

			return;
		}

		// match against the last stored features!
		const std::vector<ORBFeature>& features0 = _featuresForView.back();

		// compute putative matches
		std::vector<FeatureMatch> matches;
		FeatureMatcher::matchBruteForce( matches, orb, features0, _maxDescriptorDistance );
		//FeatureMatcher::matchWithWindow( matches, orb, features0, 150, _maxDescriptorDistance );

		std::cout << "NumMatches: " << matches.size() << std::endl;

		HomographySAC model( matches );
		RANSAC<HomographySAC> ransac( model, 1.0f /*maxreproj.*/, 0.5f /*outlierprob*/ );
		Matrix3f homography = ransac.estimate( 20000 );

		if( !checkHomography( homography ) ){
			std::cerr << "BAD HOMOGRAPHY, NOT ADDING IMAGE" << std::endl;
			return;
		}

		// TODO: not always do this!
		// compute the homography to the first image:
		_homographies.push_back( _homographies.back() );
		_homographies.back() *= homography;
		
		_images.push_back( img );
		addFeatures( orb );

		updateStitched();
	}

	bool HomographyStitching::checkHomography( const Matrix3f & homography )
	{
		float det = homography[ 0 ][ 0 ] * homography[ 1 ][ 1 ] 
				  - homography[ 1 ][ 0 ] * homography[ 0 ][ 1 ];

		if( det < 0 )
			return false;

		Vector2f v( homography[ 2 ][ 0 ], homography[ 2 ][ 1 ] );
		if( v.length() > 0.003f )
			return false;

		return true;
	}

	void HomographyStitching::addFeatures( const ORB& orb )
	{
		// add the features
		_featuresForView.push_back( std::vector<ORBFeature>() );
		std::vector<ORBFeature>& container = _featuresForView.back();
		container.reserve( orb.size() );
		for( size_t i = 0; i < orb.size(); i++ ){
			container.push_back( orb[ i ] );
		}
	}

	void HomographyStitching::updateStitched()
	{
		// using the latest added homography and image:
		// first try if the image is still big enough to capture the content
		Vector2f pt( 0, 0 );
		Vector2f p0, p1, p2, p3;

		Matrix3f homAll = _offsetTransform * _homographies.back();

		p0 = homAll * pt; 
		pt.x = _images.back().width();
		p1 = homAll * pt; 
		pt.y = _images.back().height();
		p2 = homAll * pt; 
		pt.x = 0.0f; 
		p3 = homAll * pt;

		float leftBorder = 0.0f;
		if( p0.x < leftBorder ) leftBorder = p0.x;
		if( p1.x < leftBorder ) leftBorder = p1.x;
		if( p2.x < leftBorder ) leftBorder = p2.x;
		if( p3.x < leftBorder ) leftBorder = p3.x;
		
		float rightBorder = _stitched->width();
		if( p0.x >= rightBorder ) rightBorder = p0.x;
		if( p1.x >= rightBorder ) rightBorder = p1.x;
		if( p2.x >= rightBorder ) rightBorder = p2.x;
		if( p3.x >= rightBorder ) rightBorder = p3.x;

		float topBorder = 0.0f;
		if( p0.y < topBorder ) topBorder = p0.y;
		if( p1.y < topBorder ) topBorder = p1.y;
		if( p2.y < topBorder ) topBorder = p2.y;
		if( p3.y < topBorder ) topBorder = p3.y;
		
		float bottomBorder = _stitched->height();
		if( p0.y >= bottomBorder ) rightBorder = p0.y;
		if( p1.y >= bottomBorder ) rightBorder = p1.y;
		if( p2.y >= bottomBorder ) rightBorder = p2.y;
		if( p3.y >= bottomBorder ) rightBorder = p3.y;

		if( leftBorder < 0.0f || 
		    topBorder < 0.0f ||
		    rightBorder > _stitched->width() ||
			bottomBorder > _stitched->height() ){

			// we need to resize the stitched view:
			size_t l = Math::abs( leftBorder ) + 1; 
			size_t t = Math::abs( topBorder ) + 1; 
			size_t w = l + Math::abs( rightBorder );
			size_t h = t + Math::abs( bottomBorder );

			Image* tmp = new Image( w, h, IFormat::RGBA_UINT8 );
			tmp->fill( Color::BLACK );
			
			// copy the stitched one here:
			
			tmp->copyRect( l, t, *_stitched, _stitched->rect() );
			
			delete _stitched;
			_stitched = tmp;

			_offsetTransform[ 0 ][ 2 ] += l;
			_offsetTransform[ 1 ][ 2 ] += t;

			std::cout << leftBorder << ", " << rightBorder << ", " << topBorder << ", " << bottomBorder << std::endl;
			homAll = _offsetTransform * _homographies.back();
		}

		ITransform::apply( *_stitched, _images.back(), homAll, _stitched->width(), _stitched->height() );
		std::cout << "W: " << _stitched->width() << ", H: " << _stitched->height() << std::endl;
	}
}

