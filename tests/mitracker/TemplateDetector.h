#ifndef CVT_TEMPLATE_DETECTOR_H
#define CVT_TEMPLATE_DETECTOR_H

#include <cvt/vision/FeatureMatcher.h>
#include <cvt/math/sac/RANSAC.h>
#include <cvt/math/sac/HomographySAC.h>

namespace cvt
{

	class TemplateDetector
	{
		public:
			TemplateDetector();
			~TemplateDetector();

			/* update with grayscale image */
			void updateTemplate( const Image & img );

			bool detect( Matrix3f& h, const Image& img );
			bool checkHomography( const Matrix3f & h );

		private:
			ORB*	_orbTemplate;
			//ORB2*	_orbTemplate;
			size_t	_octaves;
			float	_scaleFactor;
			uint8_t	_cornerThreshold;
			size_t	_maxFeatures;
			bool	_nms;
			float	_maxFeatureDistance;
			float	_ransacMaxReproj;
			float	_ransacOutlierProb;
	};

	inline TemplateDetector::TemplateDetector() :
		_orbTemplate( 0 ),
		_octaves( 3 ),
		_scaleFactor( 0.7f ),
		_cornerThreshold( 25 ),
		_maxFeatures( 1000 ),
		_nms( true ),
		_maxFeatureDistance( 50.0f ),
		_ransacMaxReproj( 5.0f ),
		_ransacOutlierProb( 0.2f )
	{
	}

	inline TemplateDetector::~TemplateDetector()
	{
		if( _orbTemplate )
			delete _orbTemplate;
	}

	inline void TemplateDetector::updateTemplate( const Image & img )
	{
		if( _orbTemplate )
			delete _orbTemplate;
		_orbTemplate = new ORB( img, 
		//_orbTemplate = new ORB2( img, 
								 _octaves, 
								 _scaleFactor, 
								 _cornerThreshold, 
								 _maxFeatures, 
								 _nms );
	}

	inline bool TemplateDetector::detect( Matrix3f& h, const Image& img )
	{
		//ORB2 orb( img, 
		ORB orb( img, 
				 _octaves, 
				 _scaleFactor, 
				 _cornerThreshold, 
				 _maxFeatures, 
				 _nms );

		// calculate putative matches
		std::vector<FeatureMatch> matches;

		FeatureMatcher::matchBruteForce( matches, 
										*_orbTemplate, 
										orb,
										_maxFeatureDistance );

		if( matches.size() > 10 ){
			HomographySAC model( matches );
			RANSAC<HomographySAC> ransac( model, _ransacMaxReproj, _ransacOutlierProb );

			h = ransac.estimate( 7000 );

			return checkHomography( h );
		} else {
			return false;
		}
	}


	inline bool TemplateDetector::checkHomography( const Matrix3f & homography )
	{
		float det = homography[ 0 ][ 0 ] * homography[ 1 ][ 1 ] - homography[ 1 ][ 0 ] * homography[ 0 ][ 1 ];
		if( det < 0 )
			return false;

		Vector2f v( homography[ 2 ][ 0 ], homography[ 2 ][ 1 ] );
		if( v.length() > 0.003f )
			return false;

		return true;
	}



}

#endif
