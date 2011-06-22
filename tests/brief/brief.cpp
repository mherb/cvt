#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/util/Exception.h>
#include <cvt/util/Time.h>
#include <cvt/io/Resources.h>
#include <cvt/vision/AGAST.h>
#include <cvt/vision/Brief.h>
#include <cvt/math/SL3.h>
#include <cvt/gfx/ifilter/Homography.h>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <string>
#include <vector>

using namespace cvt;

void findMatches( std::vector< std::vector<size_t> > & putativeMatches, 
                  const std::vector<BriefDescriptor> & d0, 
                  const std::vector<BriefDescriptor> & d1, 
                  int16_t distanceThreshold )
{
    for( size_t i = 0; i < d0.size(); i++ ){
        for( size_t k = 0; k < d1.size(); k++ ){
            int16_t dist = d0[ i ].distance( d1[ k ] );
            if( dist < distanceThreshold ){
                putativeMatches[ i ].push_back( k );
            }
        }
    }
}

size_t removeBorderCorners( size_t patchSize, size_t width, size_t height, std::vector<Feature2D> & f )
{
    std::vector<Feature2D>::iterator r, w;
    const std::vector<Feature2D>::const_iterator end = f.end();
    w = r = f.begin();
    
    int b0 = patchSize >> 1;
    int bw = width - b0;
    int bh = height - b0;
    
    //std::cout << "BORDER REMOVING: \n\tMin Width & height: " << b0 << "\n\tMaxWidth: " << bw << "\n\tMaxHeight: " << bh << std::endl; 
    
    size_t numGood = 0;    
    while( r != end ){
        if( r->x > b0 && r->y > b0 &&
            r->x < bw && r->y < bh ) {
            // correct pixel
            if( r != w ){
                *w = *r;
            }
            w++;
            numGood++;
        }
        r++;
    }    
    return numGood;
}

int main()
{
	Resources resources;
	std::string inputFile = resources.find( "lena_g.png" );

	try {
		Image img( inputFile );
        Image imgF;        
        img.convert( imgF, IFormat::GRAY_FLOAT );
        Image warpedF( imgF );
        Image warped( img );
        
        size_t patchSize = 17;
		Brief brief( patchSize );
		Image patch( patchSize, patchSize, IFormat::GRAY_UINT8 );
        
        SL3<float> homography;
        homography.set( Math::deg2Rad( 5.0f ), 
                        Math::deg2Rad( 2.0f ), 
                        1.1, 1.2, -30, -30, 0, 0 );
        Homography hFilter;
        Matrix3f mat;
        Color color( 0, 0, 0, 1 );
        const Eigen::Matrix3f & eMat = homography.transformation();
        mat[ 0 ][ 0 ] = eMat( 0, 0 ); mat[ 0 ][ 1 ] = eMat( 0, 1 ); mat[ 0 ][ 2 ] = eMat( 0, 2 );
        mat[ 1 ][ 0 ] = eMat( 1, 0 ); mat[ 1 ][ 1 ] = eMat( 1, 1 ); mat[ 1 ][ 2 ] = eMat( 1, 2 );
        mat[ 2 ][ 0 ] = eMat( 2, 0 ); mat[ 2 ][ 1 ] = eMat( 2, 1 ); mat[ 2 ][ 2 ] = eMat( 2, 2 );
        mat.inverseSelf();
        hFilter.apply( warpedF, imgF, mat, color );
        warpedF.convert( warped );

		AGAST agast( OAST_9_16 );
		agast.setNonMaxSuppress( true );
		agast.setThreshold( 40 );

		// detect features in input:
		std::vector<Feature2D> features0;
		agast.extract( img, features0 );
        
        // remove border pixels
        size_t numGood;
        numGood = removeBorderCorners( patchSize, img.width(), img.height(), features0 );
        
		// compute descriptors for the features:
		std::vector<BriefDescriptor> descriptors0;
		descriptors0.resize( numGood );
		
        // normal descriptor computation
        /*
        for( size_t i = 0; i < numGood; i++ ){
            patch.copyRect( 0, 0, img, 
                            features0[ i ].x, 
                            features0[ i ].y, 
                            patch.width(),
                            patch.height() );
            
            brief.descriptorForPatch( descriptors0[ i ], patch );
		}*/
        
        IntegralImage intImage( img );
        for( size_t i = 0; i < numGood; i++ ){
            brief.descriptorForPatch( descriptors0[ i ], intImage, features0[ i ] );
		}

		// detect features in warped image
        std::vector<Feature2D> features1;
		agast.extract( warped, features1 );
        // remove border pixels
        numGood = removeBorderCorners( patchSize, img.width(), img.height(), features1 );
        
        Time timer;
        timer.reset();
        
		// compute descriptors for the features:
		std::vector<BriefDescriptor> descriptors1;
		descriptors1.resize( numGood );
        
        IntegralImage intImage1( warped );
		for( size_t i = 0; i < numGood; i++ ){
            /*
            patch.copyRect( 0, 0, warped, 
                           features1[ i ].x, 
                           features1[ i ].y, 
                           patch.width(),
                           patch.height() );            
            brief.descriptorForPatch( descriptors1[ i ], patch );*/
            
            brief.descriptorForPatch( descriptors1[ i ], intImage1, features1[ i ] );
		}
        
        double t0 = timer.elapsedMilliSeconds();
        std::cout << "Descriptor Computation: " << t0 << "ms" << std::endl;

        // compare the hamming distances of the features and build up the putative matches
        // putativeMatches[ a ][ i ] -> are are the features0 indices, i the possible good matches in the other vector
        std::vector< std::vector<size_t> > putativeMatches;
        putativeMatches.resize( descriptors0.size() );
        findMatches( putativeMatches, descriptors0, descriptors1, 40 );
        
        double t1 = timer.elapsedMilliSeconds();
        std::cout << "Descriptor Matching: " << t1 - t0 << "ms" << std::endl;
        std::cout << "Overall: " << t1 << "ms" << std::endl;
        
		// evaluate the matchings using the ground truth homography
        Eigen::Matrix<float, 3, 1> point;
        Eigen::Matrix<float, 2, 1> pp;
        point[ 2 ] = 1;
        for( size_t i = 0; i < putativeMatches.size(); i++ ){
            if( putativeMatches[ i ].size() ){
                std::cout << "Putative Matches for Feature " << i;
                point[ 0 ] = features0[ i ].x;
                point[ 1 ] = features0[ i ].y;
                homography.project( pp, point );
                std::cout << " (GT: [ " << pp[ 0 ] << ", " << pp[ 1 ] << " ] : ";
                for( size_t k = 0; k < putativeMatches[ i ].size(); k++ ){
                    std::cout << "[ " << features1[ putativeMatches[ i ][ k ] ].x << ", "
                    << features1[ putativeMatches[ i ][ k ] ].y << " ] ";
                    
                }
                std::cout << std::endl;
            }
        }

	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
