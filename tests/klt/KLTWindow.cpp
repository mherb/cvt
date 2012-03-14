#include "KLTWindow.h"

namespace cvt
{
	KLTWindow::KLTWindow( VideoInput & video ) :
		_window( "KLT" ),
		_ssdSliderLabel( "SSD Threshold" ),
		_ssdSlider( 0.0f, Math::sqr( 100.0f ), 0.0f ),
		_video( video ),
		_klt( 10 ),
		_kltTimeSum( 0.0 ),
		_kltIters( 0 )
	{
		_timerId = Application::registerTimer( 10, this );
		_window.setSize( 640, 480 );
		WidgetLayout wl;
		wl.setRelativeLeftRight( 0.0f, 0.7f );
		wl.setRelativeTopBottom( 0.0f, 1.0f );
		_window.addWidget( &_imView, wl );

		wl.setRelativeLeftRight( 0.71f, 0.99f );
		wl.setAnchoredTop( 2, 20 );
		_window.addWidget( &_ssdSlider, wl );

		Delegate<void (float)> ssdDel( &_klt, &KLTType::setSSDThreshold );
		_ssdSlider.valueChanged.add( ssdDel );

		_window.setVisible( true );
		_window.update();				

		_pyramid.resize( 2 );
		_video.nextFrame();
		_video.nextFrame();
		_video.frame().convert( _pyramid[ 0 ], IFormat::GRAY_UINT8 );

		_fast.setThreshold( 30 );
		_fast.setNonMaxSuppress( true );
		_fast.setBorder( 16 );

		redetectFeatures( _pyramid[ 0 ] );
		std::cout << "Numinitial: " << _patches.size() << std::endl;

		_imView.setImage( _pyramid[ 0 ] );
		_fps = 0.0;
		_iter = 0;

	}

	KLTWindow::~KLTWindow()
	{
		Application::unregisterTimer( _timerId );
	}

	void KLTWindow::onTimeout()
	{
		_video.frame().convert( _pyramid[ 0 ], IFormat::GRAY_UINT8 );

		_kltTime.reset();

		IMapScoped<uint8_t> map( _pyramid[ 0 ] );
		size_t w = _pyramid[ 0 ].width(); 
		size_t h = _pyramid[ 0 ].height(); 

		size_t savePos = 0;
		for( size_t i = 0; i < _patches.size(); i++ ){
			if( _klt.trackPatch( _poses[ i ], *_patches[ i ], map.ptr(), map.stride(), w, h ) ){
				if( i != savePos ){
					_patches[ savePos ] = _patches[ i ];
					_poses[ savePos ]	= _poses[ i ];
				}
				savePos++;
			} else {
				delete _patches[ i ];
			}
		}

		size_t numAll = _patches.size();

		_poses.erase( _poses.begin() + savePos, _poses.end() );
		_patches.erase( _patches.begin() + savePos, _patches.end() );


		_kltTimeSum += _kltTime.elapsedMilliSeconds();
		_kltIters++;

		if( _patches.size() < 50 )
			redetectFeatures( _pyramid[ 0 ] );

		double t = _time.elapsedSeconds();
		_iter++;
		if( t > 3.0 ){
			_fps = _iter / t;
			_time.reset();
			_iter = 0;
		}

		String title;
		title.sprintf( "KLT: tracked=%d, all=%d, fps=%0.2f, avgklt=%0.2fms", savePos, numAll, _fps, _kltTimeSum/_kltIters );
		_window.setTitle( title );

		Image debug;
		_pyramid[ 0 ].convert( debug, IFormat::RGBA_UINT8 );
		drawFeatures( debug );
		_imView.setImage( debug );

		_video.nextFrame();
	}

	void KLTWindow::drawFeatures( Image & img )
	{
		{
			GFXEngineImage ge( img );
			GFX g( &ge );
			g.color() = Color::GREEN;

			Vector2f p1;
			Eigen::Vector2f p;
			Eigen::Vector2f pp;

			for( size_t i = 0; i < _patches.size(); i++ ){
				p[ 0 ] = _patches[ i ]->size() / 2;
				p[ 1 ] = _patches[ i ]->size() / 2;

				_poses[ i ].transform( pp, p );
				EigenBridge::toCVT( p1, pp );

				g.drawLine( _patches[ i ]->position(), p1 );
			}
		}
	}

	void KLTWindow::redetectFeatures( const Image & img )
	{
		std::vector<Vector2f> features;	
		VectorVector2Inserter<float> inserter( features );
		_fast.extract( img, inserter );


		for( size_t i = 0; i < _patches.size(); i++ ){
			const Vector2f & p = _patches[ i ]->position();

			std::vector<Vector2f>::iterator it = features.begin();
			while( it != features.end() ){
				if( ( p - *it ).lengthSqr() < 100.0f ){
					it = features.erase( it );
				} else {
					it++;
				}

			}
		}

		size_t oldSize = _patches.size();
		KLTPType::extractPatches( _patches, features, img );
		Matrix3f startPose;
		startPose.setIdentity();
		for( size_t i = oldSize; i < _patches.size(); i++ ){
			_poses.push_back( PoseType() );
			startPose[ 0 ][ 2 ] = _patches[ i ]->position().x - _patches[ i ]->size() / 2.0f;
			startPose[ 1 ][ 2 ] = _patches[ i ]->position().y - _patches[ i ]->size() / 2.0f; 
			_poses.back().set( startPose );
		}
	}
}
