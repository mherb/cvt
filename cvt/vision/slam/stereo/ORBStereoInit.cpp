/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#include <cvt/vision/slam/stereo/ORBStereoInit.h>

namespace cvt
{

   ORBStereoInit::ORBStereoInit( const CameraCalibration& c0, const CameraCalibration& c1, float maxEpilineDistance, float maxDescriptorDistance ) :
       DepthInitializer( c0, c1 ),
       _matcher( maxEpilineDistance, maxDescriptorDistance, c0, c1 ),
       _maxTriangError( 2.0f )
   {
   }

   void ORBStereoInit::triangulateFeatures( std::vector<DepthInitResult> & triangulated,
                                            const std::vector<Vector2f> & avoidPositionsImg0,
                                            const Image& view0, const Image& view1 )
   {
       ORB orb0( view0, 2, 0.5f, 20, 2000, true );
       ORB orb1( view1, 2, 0.5f, 20, 2000, true );

       // find stereoMatches by avoiding already found matches
       std::set<size_t> doNotUse;
       for( size_t i = 0; i < orb0.size(); i++ ){
           const Vector2f& po = orb0[ i ].pt;
           for( size_t k = 0; k < avoidPositionsImg0.size(); k++ ){
               if( ( po - avoidPositionsImg0[ k ] ).lengthSqr() < Math::sqr( 16.0f ) ){
                   doNotUse.insert( i );
                   break;
               }
           }
       }

       FeatureMatch match;
       DepthInitResult result;

       for( size_t i = 0; i < orb0.size(); ++i ){
           match.feature0 = &orb0[ i ];
           size_t idx = _matcher.matchEpipolar( match, orb1, doNotUse );
           if( match.feature1 ){
                doNotUse.insert( idx );

                // TODO: check the SAD between the two features for savety?
                //if( checkFeatureSAD( match.feature0->pt, match.feature1->pt, first, second ) )

                // correct the correspondences
                result.meas0 = match.feature0->pt;
                result.meas1 = match.feature1->pt;
                Vision::correctCorrespondencesSampson( result.meas0, result.meas1, _matcher.fundamental() );

                result.reprojectionError = triangulateSinglePoint( result.point3d, result.meas0, result.meas1, _calib0.projectionMatrix(), _calib1.projectionMatrix() );

                if( result.reprojectionError < _maxTriangError ){
                    triangulated.push_back( result );
                }
             }
        }
    }



}
