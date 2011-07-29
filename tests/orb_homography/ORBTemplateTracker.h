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
        ORBTemplateTracker( const Image & reference, size_t octaves = 4, float scaleFactor = 0.5f, uint8_t cornerThreshold = 25 ) :
            _octaves( octaves ),
            _scaleFactor( scaleFactor ),
            _cornerThreshold( cornerThreshold ),
            _maxDistance( 40.0f ),
            _lsh( &_tempFeatures )
        {

            ORB orb( reference, _octaves, _scaleFactor, _cornerThreshold, 5000 );

            for ( size_t f = 0; f < orb.size( ); f++ ) {
				bool add = true;
				for( size_t i = 0; i < orb.size(); i++ ) {
					if( f == i ) continue;
					if( orb[ f ].distance( orb[ i ] ) < _maxDistance && ( orb[ f ].pt - orb[ i ].pt).length() > 1.0f ) {
						add = false;
						break;
					}
				}
				if( add )
					_tempFeatures.push_back( orb[ f ] );
            }


            generateReferenceWarps( reference );
			std::cout << _tempFeatures.size() << " features retained" << std::endl;
            _lsh.updateFeatures( &_tempFeatures );
        }

        void genRotY( Matrix3f& mat, float angle, float fx, float fy, float d )
        {
            float rad = Math::deg2Rad( angle );
            float c = Math::cos( rad );
            float s = Math::sin( rad );

            mat[0][0] = c * fx;
            mat[0][1] = 0.0f;
            mat[0][2] = 0;
            mat[1][0] = 0.0f;
            mat[1][1] = fy;
            mat[1][2] = 0.0f;
            mat[2][0] = -s;
            mat[2][1] = 0.0f;
            mat[2][2] = d;
        }

        void genRotX( Matrix3f& mat, float angle, float fx, float fy, float d )
        {
            float rad = Math::deg2Rad( angle );
            float c = Math::cos( rad );
            float s = Math::sin( rad );

            mat[0][0] = fx;
            mat[0][1] = 0.0f;
            mat[0][2] = 0;
            mat[1][0] = 0.0f;
            mat[1][1] = c * fy;
            mat[1][2] = 0.0f;
            mat[2][0] = 0.0f;
            mat[2][1] = s;
            mat[2][2] = d;
        }


        void generateReferenceWarps( const Image & reference )
        {
            Matrix3f H, Hinv, T, Tinv;

            T.setIdentity();
            T[ 0 ][ 2 ] = reference.width() / 2.0f;
            T[ 1 ][ 2 ] = reference.height() / 2.0f;
            Tinv.setIdentity();
            Tinv[ 0 ][ 2 ] = -T[ 0 ][ 2 ];
            Tinv[ 1 ][ 2 ] = -T[ 1 ][ 2 ];


            Matrix3f R;
            Vector2f pp;

            Image warped( reference.width(), reference.height(), reference.format() );


            for ( float rot = -60.0f; rot <= 60.0f; rot += 10.0f ) {

                if( rot == 0.0f )
                    continue;

                genRotY( R, rot, 1024.0f, 1024.0f, 1024.0f );
                H = T * R * Tinv;
                Hinv = H.inverse();

                warped.fill( Color::WHITE );
                ITransform::apply( warped, reference, Hinv );

                ORB orby( warped, _octaves, _scaleFactor, _cornerThreshold, 5000 );

				for ( size_t f = 0; f < orby.size( ); f++ ) {
					bool add = true;
					for( size_t i = 0; i < orby.size(); i++ ) {
						if( f == i ) continue;
						if( orby[ f ].distance( orby[ i ] ) < _maxDistance  && ( orby[ f ].pt - orby[ i ].pt).length() > 1.0f) {
							add = false;
							break;
						}
					}
					if( add ) {
						pp = Hinv * orby[ f ].pt;
						_tempFeatures.push_back( orby[ f ] );
						_tempFeatures.back( ).pt = pp;
					}
				}

                genRotX( R, rot, 1024.0f, 1024.0f, 1024.0f );
                H = T * R * Tinv;
                Hinv = H.inverse();

                warped.fill( Color::WHITE );
                ITransform::apply( warped, reference, Hinv );

                ORB orbx( warped, _octaves, _scaleFactor, _cornerThreshold, 5000 );

				for ( size_t f = 0; f < orbx.size( ); f++ ) {
					bool add = true;
					for( size_t i = 0; i < orbx.size(); i++ ) {
						if( f == i ) continue;
						if( orbx[ f ].distance( orbx[ i ] ) < _maxDistance && ( orbx[ f ].pt - orbx[ i ].pt).length() > 1.0f) {
							add = false;
							break;
						}
					}
					if( add ) {
						pp = Hinv * orbx[ f ].pt;
						_tempFeatures.push_back( orbx[ f ] );
						_tempFeatures.back( ).pt = pp;
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

