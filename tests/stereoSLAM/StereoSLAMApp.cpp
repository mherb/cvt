#include "StereoSLAMApp.h"

#include <cvt/gui/Application.h>
#include <cvt/io/xml/XMLDocument.h>

#include <RefinedFASTTracking.h>

#include <cvt/vision/features/ORB.h>
#include <cvt/vision/features/FAST.h>

namespace cvt
{
   StereoSLAMApp::StereoSLAMApp( StereoInput* stereoInput ) :
       _stereoInput( stereoInput ),
	  _slam( new FAST( SEGMENT_10, 15, 30 ),
			 new ORB(),
			 _stereoInput->stereoCalib() ),
	  _img0( _stereoInput->left() ),
	  _img1( _stereoInput->right() ),
	  _gui(),
	   _timeIter( 0 ),
      _stepping( true ),
      _nextImage( true )
   {
      _timerId = Application::registerTimer( 10, this );

      Delegate<void ( const Image& )> d( &_gui, &SLAMGui::updateStereoView );
      _slam.newStereoView.add( d );

      Delegate<void ( const Matrix4f& )> d1( &_gui, &SLAMGui::updateCameraPose );
      _slam.newCameraPose.add( d1 );

      Delegate<void ( const SlamMap& )> mapChangeDel( &_gui, &SLAMGui::mapChanged );
      _slam.mapChanged.add( mapChangeDel );

      Delegate<void (void)> d2( this, &StereoSLAMApp::nextPressed );
      _gui.addNextFrameButtonDelegate( d2 );
      Delegate<void (void)> d3( this, &StereoSLAMApp::toggleStepping );
      _gui.addSteppingButtonDelegate( d3 );
      _gui.setStepping( _stepping );

      Delegate<void (const Image&)> d4( &_gui, &SLAMGui::setCurrentImage );
      _slam.trackedFeatureImage.add( d4 );

      Delegate<void (size_t)> dnpts( &_gui, &SLAMGui::setNumTrackedFeatures );
      _slam.numTrackedPoints.add( dnpts );

      Delegate<void (void)> d5( this, &StereoSLAMApp::saveMap );
      _gui.setSaveButtonDelegate( d5 );

      Delegate<void (void)> cM( this, &StereoSLAMApp::clearMap );
      _gui.addMapClearDelegate( cM );

      Delegate<void (void)> kfAddDel( this, &StereoSLAMApp::saveImages );
      _slam.keyframeAdded.add( kfAddDel );
   }

   StereoSLAMApp::~StereoSLAMApp()
   {
      Application::unregisterTimer( _timerId );      
   }

   void StereoSLAMApp::onTimeout()
   {
      if( !_stepping || ( _stepping && _nextImage ) ){
         _stereoInput->nextFrame( 10 );
         _nextImage = false;

         _stereoInput->left().convert( _img0, IFormat::GRAY_UINT8 );
         _stereoInput->right().convert( _img1, IFormat::GRAY_UINT8 );
         _slam.newImages( _img0, _img1 );
      }

      _timeIter++;
      if( _time.elapsedSeconds() > 3.0f ){
         _gui.setFPS( 1000.0f * _timeIter / _time.elapsedMilliSeconds() );
         _time.reset();
         _timeIter = 0;
      }
   }

   void StereoSLAMApp::saveMap()
   {
      std::cout << "Saving map ...";
      _slam.map().save( "map.xml" );
      std::cout << " done" << std::endl;
   }

   void StereoSLAMApp::toggleStepping()
   {
      _stepping = !_stepping;
      _gui.setStepping( _stepping );
   }

   void StereoSLAMApp::clearMap()
   {
      _slam.clear();
   }

   void StereoSLAMApp::saveImages()
   {
      static size_t keyframeIter = 0;
//      String savename;
//      savename.sprintf( "left_keyframe_%05d.cvtraw", keyframeIter );
//      _stereoInput->left().save( savename );
//      savename.sprintf( "right_keyframe_%05d.cvtraw", keyframeIter );
//      _stereoInput->right().save( savename );
      keyframeIter++;
   }
}
