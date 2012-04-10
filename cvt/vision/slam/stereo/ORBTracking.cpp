/*
         CVT - Computer Vision Tools Library

    Copyright (c) 2012, Philipp Heise, Sebastian Klose

   THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.
 */
#include <cvt/vision/slam/stereo/ORBTracking.h>

namespace cvt
{
   ORBTracking::ORBTracking() :
      _maxDescDistance( 60 ),
      _windowRadius( 25 ),
      _orbOctaves( 3 ),
      _orbScaleFactor( 0.5f ),
      _orbCornerThreshold( 17 ),
      _orbMaxFeatures( 1000 ),
      _orbNonMaxSuppression( true ),
      _orb0( _orbOctaves, _orbScaleFactor, _orbCornerThreshold, _orbMaxFeatures, _orbNonMaxSuppression )
   {
   }

   ORBTracking::~ORBTracking()
   {
   }

   void ORBTracking::trackFeatures( PointSet2d&                         trackedPositions,
                                    std::vector<size_t>&		trackedFeatureIds,
                                    const std::vector<Vector2f>&	predictedPositions,
                                    const std::vector<size_t>&		predictedIds,
                                    const Image&			img )
   {
      // create the ORB
      _orb0.update( img );

      // we want to find the best matching orb feature from current, that lies
      // within a certain distance from the "predicted" position
      std::vector<size_t>::const_iterator currentId = predictedIds.begin();
      std::vector<size_t>::const_iterator tEnd = predictedIds.end();
      std::vector<Vector2f>::const_iterator pred = predictedPositions.begin();

      // keep track of already assigned indices to avoid double associations
      _orb0MatchedIds.clear();
      while( currentId != tEnd ){
         FeatureMatch m;
         const ORBFeature & desc = _descriptors.descriptor( *currentId );
         m.feature0 = &desc;
         int matchedIdx = matchInWindow( m, *pred, _orb0, _orb0MatchedIds );
         if( matchedIdx != -1 ){
            _orb0MatchedIds.insert( ( size_t )matchedIdx );
            trackedPositions.add( Vector2d( m.feature1->pt.x, m.feature1->pt.y ) );
            trackedFeatureIds.push_back( *currentId );
         }
         ++currentId;
         ++pred;
      }
   }

   void ORBTracking::addFeatureToDatabase( const Vector2f & f, size_t id )
   {
       // this is odd, we need to search the closest feature in orb0
       size_t closestIdx = 0;
       float  distsqr = Math::MAXF;
       for( size_t i = 0; i < _orb0.size(); ++i ){
           float currd = ( _orb0[ i ].pt - f ).lengthSqr();
           if( currd < distsqr ){
               closestIdx = i;
               distsqr = currd;
           }
       }

       _descriptors.addDescriptor( _orb0[ closestIdx ], id );
   }

   int ORBTracking::matchInWindow( FeatureMatch& match, const Vector2f & p, const ORB & orb, const std::set<size_t> & used ) const
   {
      const ORBFeature * f = (ORBFeature*)match.feature0;
      match.feature1 = 0;
      match.distance = _maxDescDistance;
      size_t currDist;
      const std::set<size_t>::const_iterator usedEnd = used.end();
      size_t matchedId = 0;
      for( size_t i = 0; i < orb.size(); i++ ){
         if( used.find( i ) == usedEnd ){
            if( ( p - orb[ i ].pt ).length() < _windowRadius ){
               // try to match
               currDist = f->distance( orb[ i ] );
               if( currDist < match.distance ){
                  match.feature1 = &orb[ i ];
                  match.distance = currDist;
                  matchedId = i;
               }
            }
         }
      }

      // to ensure unique matches
      if( match.distance < _maxDescDistance ){
         return matchedId;
      }
      return -1;
   }

   void ORBTracking::clear()
   {
      _descriptors.clear();
      _orb0MatchedIds.clear();
   }
}

