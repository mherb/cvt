/*
 * File:   ORBTemplateTracker.h
 * Author: sebi
 *
 * Created on July 21, 2011, 2:10 PM
 */

#ifndef CVT_ORBTEMPLATETRACKER_H
#define	CVT_ORBTEMPLATETRACKER_H

#include <cvt/gfx/Image.h>
#include <cvt/vision/ORB.h>

namespace cvt
{
    class ORBTemplateTracker
    {
      public:
        ORBTemplateTracker( const Image & reference, size_t octaves=3, float scaleFactor = 0.7f, uint8_t cornerThreshold=25 ) :
            _octaves( octaves ),
            _scaleFactor( scaleFactor ),
            _cornerThreshold( cornerThreshold ),
            _maxDistance( 40.0f ),
            _referenceORB( reference, _octaves, _scaleFactor, _cornerThreshold )
        {
        }


        bool estimate( Matrix3f & homography, const Image & img )
        {
            ORB currOrb( img, _octaves, _scaleFactor, _cornerThreshold );

            std::vector<FeatureMatch> matches;
            findMatches( matches, currOrb );

            HomographySAC model( matches );
            RANSAC<HomographySAC> ransac( model, 5.0f /*maxreproj.*/, 0.3f /*outlierprob*/ );

            homography = ransac.estimate( 1000 );

            return true;
        }

        void findMatches( std::vector<FeatureMatch> & matches, const ORB & other )
        {
            FeatureMatch m;

            for ( size_t i = 0; i < _referenceORB.size( ); i++ ) {
                m.feature0 = &_referenceORB[ i ];
                m.distance = _maxDistance;
                for ( size_t k = 0; k < other.size( ); k++ ) {
                    size_t dist = _referenceORB[ i ].distance( other[ k ] );

                    if ( dist < m.distance ) {
                        m.distance = dist;
                        m.feature1 = &other[ k ];
                    }
                }

                if ( m.distance < _maxDistance ) {
                    matches.push_back( m );
                }
            }
        }

        void setMaxDescriptorDistance( float maxDistance )
        {
            _maxDistance = maxDistance;
        }

      private:
        size_t  _octaves;
        float   _scaleFactor;
        uint8_t _cornerThreshold;
        float   _maxDistance;

        ORB     _referenceORB;
    };
}


#endif	/* ORBTEMPLATETRACKER_H */

