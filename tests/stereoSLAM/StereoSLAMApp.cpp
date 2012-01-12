#include "StereoSLAMApp.h"

#include <cvt/gui/Application.h>
#include <cvt/io/xml/XMLDocument.h>

namespace cvt
{
	StereoSLAMApp::StereoSLAMApp( const std::vector<VideoInput*> & cams,
										 const CameraCalibration & c0,
										 const CameraCalibration & c1 ):
		_cams( cams ),
		_slam( c0, cams[ 0 ]->width(), cams[ 0 ]->height(), c1, cams[ 1 ]->width(), cams[ 1 ]->height() ),
		_img0( cams[ 0 ]->width(), cams[ 0 ]->height(), cams[ 0 ]->format() ),
		_img1( cams[ 1 ]->width(), cams[ 1 ]->height(), cams[ 1 ]->format() ),
		_stepping( true ),
		_nextImage( true )
	{
		_timerId = Application::registerTimer( 5, this );

		Delegate<void ( const StereoSLAM::ORBData* )> d( &_gui, &SLAMGui::updateStereoView );
		_slam.newORBData.add( d );

		Delegate<void ( const Matrix4f& )> d1( &_gui, &SLAMGui::updateCameraPose );
		_slam.newCameraPose.add( d1 );

		Delegate<void ( const SlamMap& )> mapChangeDel( &_gui, &SLAMGui::mapChanged );
		_slam.mapChanged.add( mapChangeDel );
		
		Delegate<void (void)> d2( this, &StereoSLAMApp::nextPressed );
		_gui.addNextFrameButtonDelegate( d2 );
		Delegate<void (void)> d3( this, &StereoSLAMApp::toggleStepping );
		_gui.addSteppingButtonDelegate( d3 );
		_gui.setStepping( _stepping );
		
		Delegate<void (const PointSet2d&)> d4( &_gui, &SLAMGui::updateTrackedPoints );
		_slam.trackedPoints.add( d4 );
		
		Delegate<void (void)> d5( this, &StereoSLAMApp::saveMap );
		_gui.setSaveButtonDelegate( d5 );
		
		Delegate<void (void)> cM( this, &StereoSLAMApp::clearMap );
		_gui.addMapClearDelegate( cM );
	}

	StereoSLAMApp::~StereoSLAMApp()
	{
		Application::unregisterTimer( _timerId );
		for( size_t i = 0; i < _cams.size(); i++ ){
			delete _cams[ i ];
		}		
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

			_gui.setCurrentImage( _slam.undistorted( 0 ) );
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
}
