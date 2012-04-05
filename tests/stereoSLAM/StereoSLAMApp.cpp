#include "StereoSLAMApp.h"

#include <cvt/gui/Application.h>
#include <cvt/io/xml/XMLDocument.h>

#include <cvt/vision/slam/stereo/ORBTracking.h>
#include <cvt/vision/slam/stereo/KLTTracking.h>

namespace cvt
{
	StereoSLAMApp::StereoSLAMApp( const std::vector<VideoInput*> & cams,
										 const CameraCalibration & c0,
										 const CameraCalibration & c1 ):
		_cams( cams ),
		_featureTracking( new ORBTracking( c0, c1 ) ),
		//_featureTracking( new KLTTracking( c0, c1 ) ),
		_slam( _featureTracking, c0, cams[ 0 ]->width(), cams[ 0 ]->height(), c1, cams[ 1 ]->width(), cams[ 1 ]->height() ),
		_img0( cams[ 0 ]->width(), cams[ 0 ]->height(), cams[ 0 ]->format() ),
		_img1( cams[ 1 ]->width(), cams[ 1 ]->height(), cams[ 1 ]->format() ),
		_stepping( false ),
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
		_featureTracking->debugImage.add( d4 );

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
		for( size_t i = 0; i < _cams.size(); i++ ){
			delete _cams[ i ];
		}

		delete _featureTracking;
	}

	void StereoSLAMApp::onTimeout()
	{
		if( !_stepping || ( _stepping && _nextImage ) ){
			_cams[ 0 ]->nextFrame();
			_cams[ 1 ]->nextFrame();
			_nextImage = false;

			_cams[ 0 ]->frame().convert( _img0, IFormat::GRAY_UINT8 );
			_cams[ 1 ]->frame().convert( _img1, IFormat::GRAY_UINT8 );
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
		std::cout << "Saving map" << std::endl;
		XMLDocument doc;
		doc.addNode( _slam.map().serialize() );
		doc.save( "map.xml" );
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
		//static size_t keyframeIter = 0;
		//String savename;
		//savename.sprintf( "ueye_4002738790_keyframe_%05d.cvtraw", keyframeIter );
		//_cams[ 0 ]->frame().save( savename );
		//savename.sprintf( "ueye_4002738788_keyframe_%05d.cvtraw", keyframeIter );
		//_cams[ 1 ]->frame().save( savename );
		//keyframeIter++;
	}
}
