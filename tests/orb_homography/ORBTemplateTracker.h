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
            _maxDistance( 50.0f ),
            _lsh( &_tempFeatures )
		{
			std::vector<ORBFeature> features;
			generateReferenceWarps( reference, features );
			std::cout << "Warped ... " << std::endl;

			std::cout << features.size() << " features found" << std::endl;
			for( size_t f = 0; f < features.size(); f++ ) {
				bool add = true;
				for( size_t i = 0; i < features.size(); i++ ) {
					if( i == f ) continue;
					if( features[ f ].distance( features[ i ] ) < _maxDistance && ( features[ f ].pt - features[ i ].pt ).length() > 1.0f ) {
						add = false;
						break;
					}
				}

				if( add ) {
					_tempFeatures.push_back( features[ f ] );
				}

			}

			std::cout << _tempFeatures.size() << " features retained" << std::endl;
			_lsh.updateFeatures( &_tempFeatures );
		}

        void generateReferenceWarps( const Image & reference, std::vector<ORBFeature>& features )
		{
			Matrix3f H, Hinv, T, Tinv;

			T.setIdentity();
			T[ 0 ][ 2 ] = ( reference.width() - 1 ) / 2.0f;
			T[ 1 ][ 2 ] = ( reference.height() - 1 ) / 2.0f;
			Tinv.setIdentity();
			Tinv[ 0 ][ 2 ] = -T[ 0 ][ 2 ];
			Tinv[ 1 ][ 2 ] = -T[ 1 ][ 2 ];


			Matrix3f R;
			Vector2f pp;

			Image warped( reference.width(), reference.height(), reference.format() );

			for ( float yrot = -50.0f; yrot <= 50.0f; yrot += 10.0f ) {
				for ( float xrot = -50.0f; xrot <= 50.0f; xrot += 10.0f ) {

					R.setProjectedRotation( Math::deg2Rad( xrot), Math::deg2Rad( yrot ), 0.0f );
					H = T * R * Tinv;
					Hinv = H.inverse();

					warped.fill( Color::WHITE );
					ITransform::apply( warped, reference, H );

					ORB orb( warped, _octaves, _scaleFactor, _cornerThreshold, 5000 );

					for ( size_t f = 0; f < orb.size( ); f++ ) {
						pp = Hinv * orb[ f ].pt;
						bool add = true;
						/* only add features not already in the set */
						for( size_t i = 0; i < features.size(); i++ ) {
							if( orb[ f ].distance( features[ i ] ) < _maxDistance && ( features[ i ].pt - pp ).length() < 1.0f ) {
								add = false;
								break;
							}
						}
						if( add ) {
							features.push_back( orb[ f ] );
							features.back( ).pt = pp;
						}
					}
				}
			}
		}

        // test if the estimated homography makes sense!
        bool checkHomography( const Matrix3f & homography )
        {
            float det = homography[ 0 ][ 0 ] * homography[ 1 ][ 1 ] - homography[ 1 ][ 0 ] * homography[ 0 ][ 1 ];
            if( det < 0 )
                return false;

            Vector2f v( homography[ 2 ][ 0 ], homography[ 2 ][ 1 ] );
            if( v.length() > 0.003f )
                return false;

            return true;
        }


        bool estimate( Matrix3f & homography, const Image & _img )
        {
			Image img;
			_img.scale( img, _img.width() * 2, _img.height() * 2, IScaleFilterBilinear() );

            ORB currOrb( img, _octaves, _scaleFactor, _cornerThreshold, 3000 );

			//adaptCornerThreshold( currOrb.size() );

            std::vector<FeatureMatch> matches;
            findMatches( matches, currOrb );

			if( matches.size() > 10 ){
				HomographySAC model( matches );
				RANSAC<HomographySAC> ransac( model, 5.0f /*maxreproj.*/, 0.5f /*outlierprob*/ );

				homography = ransac.estimate( 7000 );

				return checkHomography( homography );
			} else {
				return false;
			}
        }

		void adaptCornerThreshold( size_t numFeatures )
		{
			if( numFeatures < 1000 && _cornerThreshold > 20 ){
				_cornerThreshold--;
				std::cout << "New Threshold: " << (int)_cornerThreshold << std::endl;
			} else if ( numFeatures > 1100 && _cornerThreshold < 80 ){
				_cornerThreshold++;
				std::cout << "New Threshold: " << (int)_cornerThreshold << std::endl;
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

        LSH<10, 2>   _lsh;

    };
}


#endif	/* ORBTEMPLATETRACKER_H */

