/* 
 * File:   HomographyStitching.cpp
 * Author: sebi
 * 
 * Created on February 27, 2012, 2:57 PM
 */

#include "HomographyStitching.h"
#include "PlaneCompositingSurface.h"
#include "CylindricalCompositingSurface.h"
#include <cvt/math/sac/HomographySAC.h>
#include <cvt/math/sac/RANSAC.h>
#include <cvt/vision/FeatureMatcher.h>

namespace cvt {

    HomographyStitching::HomographyStitching( const Matrix3f& k ) :
		_intrinsics( k ),
		_intrinsicsInv( k.inverse() ),
		//_surface( new PlaneCompositingSurface() ),
		_surface( new CylindricalCompositingSurface( k[ 0 ][ 0 ] ) ),
		_octaves( 4 ),
		_scale( 0.5f ),
		_fastCornerThreshold( 25 ),
		_maxNumFeatures( 0 ),
		_nonMaxSuppress( false ),
		_maxDescriptorDistance( 40 )
    {
    }

    HomographyStitching::~HomographyStitching()
    {
		delete _surface;
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
			// identity rotation as well
			_rotations.push_back( _homographies.back() );

			_surface->addImage( img, _intrinsics, _rotations.back() );
			return;
		}

		// match against the last stored features!
		const std::vector<ORBFeature>& features0 = _featuresForView.back();

		// compute putative matches
		std::vector<FeatureMatch> matches;
		FeatureMatcher::matchBruteForce( matches, orb, features0, _maxDescriptorDistance );
		//FeatureMatcher::matchWithWindow( matches, orb, features0, 150, _maxDescriptorDistance );

		HomographySAC model( matches );
		RANSAC<HomographySAC> ransac( model, 0.5f /*maxreproj.*/, 0.4f /*outlierprob*/ );
		Matrix3f homography = ransac.estimate( 10000 );

		if( !checkHomography( homography ) ){
			std::cerr << "BAD HOMOGRAPHY, NOT ADDING IMAGE" << std::endl;
			return;
		}

		// TODO: not always do this!
		// compute the homography to the first image:
		_homographies.push_back( _homographies.back() );
		_homographies.back() *= homography;

		// extract the rotation and compute the overall rotation
		Matrix3f rotation;
		rotationFromHomography( rotation, homography );
		_rotations.push_back( _rotations.back() );
		_rotations.back() *= rotation;

		_images.push_back( img );
		addFeatures( orb );
			
		_surface->addImage( img, _intrinsics, _rotations.back() );
	}

	bool HomographyStitching::checkHomography( const Matrix3f & homography )
	{
		float det = homography[ 0 ][ 0 ] * homography[ 1 ][ 1 ] 
				  - homography[ 1 ][ 0 ] * homography[ 0 ][ 1 ];

		if( det < 0 )
			return false;

		Vector2f v( homography[ 2 ][ 0 ], homography[ 2 ][ 1 ] );
		if( v.length() > 0.03f )
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


	void HomographyStitching::rotationFromHomography( Matrix3f& r, const Matrix3f& hom ) const
	{
		r = _intrinsicsInv * hom * _intrinsics; 
	}

}

