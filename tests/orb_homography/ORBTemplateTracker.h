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
#include <cvt/vision/LSH.h>
#include <cvt/gfx/ifilter/ITransform.h>

namespace cvt
{
    class ORBTemplateTracker
    {
      public:
        ORBTemplateTracker( const Image & reference, size_t octaves = 4, float scaleFactor = 0.7f, uint8_t cornerThreshold = 25 ) :
            _octaves( octaves ),
            _scaleFactor( scaleFactor ),
            _cornerThreshold( cornerThreshold ),
            _maxDistance( 40.0f ),
            _lsh( &_tempFeatures )
        {
            ORB orb( reference, _octaves, _scaleFactor, _cornerThreshold, 200 );

            for ( size_t f = 0; f < orb.size( ); f++ ) {
                _tempFeatures.push_back( orb[ f ] );
            }
            generateReferenceWarps( reference, 15.0f, 25.0f, 75.0f );
            _lsh.updateFeatures( &_tempFeatures );
        }


        void generateReferenceWarps( const Image & reference, float templateWidth, float templateHeight, float depth )
        {
            Matrix3f H, Hinv;

            Matrix3f K( 100, 0.0f, reference.width() / 2.0f,
                        0.0f, 100, reference.height() / 2.0f,
                        0.0f, 0.0f, 1.0f );
            Matrix3f Kinv = K.inverse();

            Matrix3f Rx, Ry;
            Vector2f pp;

            Line2Df l;

            Image warped( reference.width(), reference.height(), reference.format() );

            for( float rotX = -20.0f; rotX <= 20.0f; rotX+= 5.0f ){
                Rx.setRotationX( Math::deg2Rad( rotX ) );
                for( float rotY = -20.0f; rotY <= 20.0f; rotY += 5.0f ){
                    Ry.setRotationY( Math::deg2Rad( rotY ) );
                    H = K * Rx * Ry * Kinv;

                    H.setIdentity();
                    H[ 2 ].set( 1, 0, 10000 );

                    Hinv = H.inverse();

                    warped.fill( Color::WHITE );

                    ITransform::apply( warped, reference, Hinv );

                    warped.save( "bla.png" );
                    getchar();

                    ORB orb( warped, _octaves, _scaleFactor, _cornerThreshold, 150 );

                    for( size_t f = 0; f < orb.size(); f++ ){
                        pp = Hinv * orb[ f ].pt;
                        _tempFeatures.push_back( orb[ f ] );
                        _tempFeatures.back().pt = pp;
                    }
                }
            }
        }


        bool estimate( Matrix3f & homography, const Image & _img )
        {
			Image img;
			_img.scale( img, _img.width() * 2, _img.height() * 2, IScaleFilterBilinear() );

            ORB currOrb( img, _octaves, _scaleFactor, _cornerThreshold, 1500 );

            std::vector<FeatureMatch> matches;
            findMatches( matches, currOrb );

			if( matches.size() > 10 ){
				HomographySAC model( matches );
				RANSAC<HomographySAC> ransac( model, 5.0f /*maxreproj.*/, 0.5f /*outlierprob*/ );

				homography = ransac.estimate( 7000 );

				return true;
			} else {
				return false;
			}
        }


        void findMatches( std::vector<FeatureMatch> & matches, const ORB & other )
        {
            FeatureMatch m;

            size_t distance = 0;
            int idx;

            for( size_t i = 0; i < other.size(); i++ ){
                const ORBFeature & f = other[ i ];

                idx = _lsh.find( f, distance, _maxDistance );

                if( idx > 0  ){
                    m.feature0 = &_tempFeatures[ idx ];
                    m.feature1 = &f;
                    m.distance = distance;
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

        std::vector<ORBFeature> _tempFeatures;

        LSH<8, 2>   _lsh;

    };
}


#endif	/* ORBTEMPLATETRACKER_H */

