/*
   The MIT License (MIT)

   Copyright (c) 2011 - 2013, Philipp Heise and Sebastian Klose

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include <cvt/vision/slam/SlamMap.h>

#include <set>

namespace cvt
{
   SlamMap::SlamMap() :
      _numMeas( 0 )
   {
   }

   SlamMap::~SlamMap()
   {
   }

   void SlamMap::clear()
   {
      _keyframes.clear();
      _features.clear();
      _numMeas = 0;
   }

   size_t SlamMap::addKeyframe( const Eigen::Matrix4d& pose )
   {
      size_t id = _keyframes.size();
      _keyframes.push_back( Keyframe( pose, id ) );
      return id;
   }


   size_t SlamMap::addFeature( const MapFeature& world )
   {
      _features.push_back( world );
      return _features.size()-1;
   }

   size_t SlamMap::addFeatureToKeyframe( const MapFeature& world,
                                         const MapMeasurement& feature,
                                         size_t keyframeId )
   {
      size_t pointId = addFeature( world );
      addMeasurement( pointId, keyframeId, feature );
      return pointId;
   }

   void SlamMap::addMeasurement( size_t pointId,
                                 size_t keyframeId,
                                 const  MapMeasurement& meas )
   {
      _features[ pointId ].addPointTrack( keyframeId );
      _keyframes[ keyframeId ].addFeature( meas, pointId );
      _numMeas++;
   }

   int SlamMap::findClosestKeyframe( const Eigen::Matrix4d& worldT ) const
   {
      double nearest = Math::MAXF;
      int kf = -1;

      for( size_t i = 0; i < _keyframes.size(); i++ ){
         double dist = _keyframes[ i ].distance( worldT );
         if( dist < nearest ){
            nearest = dist;
            kf = i;
         }
      }
      return kf;
   }

   void SlamMap::selectVisibleFeatures( std::vector<size_t> & visibleFeatureIds,
                                        std::vector<Vector2f> & projections,
                                        const Eigen::Matrix4d&	cameraPose /* this is the rig pose*/,
                                        const CameraCalibration& camCalib,
                                        double maxDistance ) const
   {
      Eigen::Vector4d pointInCam;
      Vector4f pic;
      Vector4f sp;
      Vector2f pointInScreen;

      // this is a hack: we should store the image width/height with the calibration object!
      size_t w = camCalib.width();
      size_t h = camCalib.height();

      std::set<size_t> usedPoints;
      for( size_t i = 0; i < _keyframes.size(); i++ ){
          double kfDistance = _keyframes[ i ].distance( cameraPose );          
          if( kfDistance < maxDistance ){
            // check if the points of this keyframe project to this camera
            const Keyframe& kf = _keyframes[ i ];

            Keyframe::MeasurementIterator iter = kf.measurementsBegin();
            const Keyframe::MeasurementIterator measEnd = kf.measurementsEnd();

            while( iter != measEnd ){
               size_t fId = iter->first;
               const MapFeature& feature = _features[ fId ];               

               if( usedPoints.find( fId ) == usedPoints.end() ){
                  pointInCam = cameraPose * feature.estimate();
                  pointInCam /= pointInCam[ 3 ];

                  if( pointInCam[ 2 ] > 0.0 ){
                     pic[ 0 ] = ( float )pointInCam[ 0 ];
                     pic[ 1 ] = ( float )pointInCam[ 1 ];
                     pic[ 2 ] = ( float )pointInCam[ 2 ];
                     pic[ 3 ] = ( float )pointInCam[ 3 ];

                     // project it to the screen:
                     sp = camCalib.projectionMatrix() * pic;
                     pointInScreen.x = sp.x / sp.z;
                     pointInScreen.y = sp.y / sp.z;                     

                     if( pointInScreen.x > 0 &&
                         pointInScreen.x < w &&
                         pointInScreen.y > 0 &&
                         pointInScreen.y < h ){
                        usedPoints.insert( fId );
                        visibleFeatureIds.push_back( fId );
                        projections.push_back( pointInScreen );
                     }
                  }
               }
               ++iter;
            }
         }
      }
   }

   void SlamMap::deserialize( XMLNode* node )
   {
      if( node->name() != "SlamMap" ){
         throw CVTException( "This is not a SlamMap node" );
      }

      // get intrinsics:
      Matrix3d K;
      K = Matrix3d::fromString( node->childByName( "Intrinsics" )->child( 0 )->value() );
      EigenBridge::toEigen( _intrinsics, K );

      XMLNode* keyframes = node->childByName( "Keyframes" );
      if( keyframes == NULL ){
         throw CVTException( "No Keyframes in MapFile!" );
      }

      size_t numKF = keyframes->childSize();
      _keyframes.resize( numKF );
      _numMeas = 0;
      for( size_t i = 0; i < _keyframes.size(); i++ ){
         XMLNode* kfNode = keyframes->child( i );

         // get the id:
         size_t kfId = kfNode->childByName( "id" )->value().toInteger();

         _keyframes[ kfId ].deserialize( kfNode );
         _numMeas += _keyframes[ kfId ].numMeasurements();
      }

      XMLNode* featureNodes = node->childByName( "MapFeatures" );
      if( featureNodes == NULL ){
         throw CVTException( "No Features in MapFile!" );
      }

      _features.resize( featureNodes->childSize() );
      for( size_t i = 0; i < _features.size(); i++ ){
         XMLNode* fNode = featureNodes->child( i );
         _features[ i ].deserialize( fNode );
      }
   }

   XMLNode* SlamMap::serialize() const
   {
      XMLElement* mapNode = new XMLElement( "SlamMap");

      // Intrinsics of the Keyframe images
      {
         Matrix3d K;
         EigenBridge::toCVT( K, _intrinsics );
         XMLElement* camIntrinsics = new XMLElement( "Intrinsics" );
         camIntrinsics->addChild( new XMLText( K.toString() ) );
         mapNode->addChild( camIntrinsics );
      }

      // the keyframes: serialize each one
      XMLElement* keyframeNodes = new XMLElement( "Keyframes" );
      for( size_t i = 0; i < _keyframes.size(); i++ ){
         keyframeNodes->addChild( _keyframes[ i ].serialize() );
      }
      mapNode->addChild( keyframeNodes );


      // the mapfeatures
      XMLElement* mapFeatureNodes = new XMLElement( "MapFeatures" );
      for( size_t i = 0; i < _features.size(); i++ ){
         mapFeatureNodes->addChild( _features[ i ].serialize() );
      }
      mapNode->addChild( mapFeatureNodes );

      return mapNode;
   }

   void SlamMap::load( const String& filename )
   {
       XMLDocument xmlDoc;
       xmlDoc.load( filename );

       XMLNode* node = xmlDoc.nodeByName( "SlamMap" );
       this->deserialize( node );
   }

   void SlamMap::save( const String& filename ) const
   {
       XMLDocument doc;
       doc.addNode( this->serialize() );
       doc.save( filename );
   }
}
