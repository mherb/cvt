#include "KLTWindow.h"

namespace cvt
{
	KLTWindow::KLTWindow( VideoInput & video ) :
		_window( "KLT" ),
		_ssdSliderLabel( "SSD Threshold" ),
		_ssdSlider( 0.0f, Math::sqr( 100.0f ), 0.0f ),
		_stepButton( "Enable Freerun" ),
		_nextButton( "Next Frame" ),
		_stepping( true ),
		_next( false ),
		_video( video ),
		_klt( 20 ),
		_kltTimeSum( 0.0 ),
		_kltIters( 0 ),
		_redetectThreshold( 20 ),
		_gridFilter( 20, video.width(), video.height() )
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

		wl.setAnchoredTop( 25, 20 );
		_window.addWidget( &_ssdSliderLabel, wl );

		wl.setAnchoredTop( 47, 20 );
		_window.addWidget( &_stepButton, wl );
		wl.setAnchoredTop( 70, 20 );
		_window.addWidget( &_nextButton, wl );

		Delegate<void ()> stepDel( this, &KLTWindow::steppingPressed );
		Delegate<void ()> nextDel( this, &KLTWindow::nextPressed );

		_stepButton.clicked.add( stepDel );
		_nextButton.clicked.add( nextDel );

		Delegate<void (float)> ssdDel( this, &KLTWindow::ssdThresholdChanged );
		_ssdSlider.valueChanged.add( ssdDel );
		_ssdSlider.setValue( 50.0f );

		_window.setVisible( true );
		_window.update();				

		_pyramid.resize( 2 );
		_video.nextFrame();
		_video.nextFrame();

		Image gray( _video.width(), _video.height(), IFormat::GRAY_UINT8 );
		_pyramid[ 0 ].reallocate( _video.width(), _video.height(), IFormat::GRAY_UINT8 );
		_video.frame().convert( gray, IFormat::GRAY_UINT8 );

		const IKernel & horiz = IKernel::GAUSS_HORIZONTAL_3;
		const IKernel & vert = IKernel::GAUSS_VERTICAL_3;
		gray.convolve( _pyramid[ 0 ], horiz, vert );

		_fast.setThreshold( 14 );
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

	void KLTWindow::ssdThresholdChanged( float val )
	{
		_klt.setSSDThreshold( val );
		String label;
		label.sprintf( "Treshold: %0.1f", val );
		_ssdSliderLabel.setLabel( label );
	}

	void KLTWindow::nextPressed()
	{
		_next = true;
	}

	void KLTWindow::steppingPressed()
	{
		String label;
		if( _stepping ){
			label = "Enable stepping";
			_nextButton.setVisible( false );
		} else {
			label = "Enable freerun";
			_nextButton.setVisible( true );
		}
		_stepping = !_stepping;
		_stepButton.setLabel( label );
	}

	void KLTWindow::onTimeout()
	{
		if( _stepping && !_next )
			return;
		_next = false;
		// convert to grayscale
		_video.frame().convert( _pyramid[ 0 ], IFormat::GRAY_UINT8 );


		_kltTime.reset();


		size_t savePos = 0;
		size_t numAll = 0;
		{
			// map the image
			IMapScoped<uint8_t> map( _pyramid[ 0 ] );
			size_t w = _pyramid[ 0 ].width();
			size_t h = _pyramid[ 0 ].height();

			// track all patches
			for( size_t i = 0; i < _patches.size(); i++ ){
				if( _klt.trackPatch( *_patches[ i ], map.ptr(), map.stride(), w, h ) ){
					if( i != savePos ){
						_patches[ savePos ] = _patches[ i ];
					}
					savePos++;
				} else {
					delete _patches[ i ];
				}
			}
			numAll = _patches.size();
			_patches.erase( _patches.begin() + savePos, _patches.end() );
		}

		_kltTimeSum += _kltTime.elapsedMilliSeconds();
		_kltIters++;

		if( _patches.size() < _redetectThreshold )
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

			Vector2f c;
			Recti r;
			size_t offset = PATCH_SIZE >> 1;
			r.width = PATCH_SIZE;
			r.height = PATCH_SIZE;
			for( size_t i = 0; i < _patches.size(); i++ ){
				_patches[ i ]->currentCenter( c );

				r.x = c.x - offset;
				r.y = c.y - offset;
				g.drawRect( r );

			}
		}
	}

	void KLTWindow::redetectFeatures( const Image & img )
	{
		std::vector<Feature2Df> features;	
		VectorFeature2DInserter<float> inserter( features );
		_fast.extract( img, inserter );

		_gridFilter.clear();
		for( size_t i = 0; i < features.size(); i++ ){
			_gridFilter.addFeature( &features[ i ] );
		}

		std::vector<const Feature2Df*> filtered;
		_gridFilter.gridFilteredFeatures( filtered, 800 );

		std::vector<Vector2f> filteredUnique;

		std::vector<Vector2f> patchPositions;
		patchPositions.resize( _patches.size() );
		for( size_t i = 0; i < _patches.size(); i++ ){
			_patches[ i ]->currentCenter( patchPositions[ i ] );
		}
		
		for( size_t k = 0; k < filtered.size(); k++ ){
			bool good = true;
			for( size_t i = 0; i < patchPositions.size(); i++ ){
				if( ( patchPositions[ i ] - filtered[ k ]->pt ).lengthSqr() < 100.0f ){
					good = false;
					break;
				}
			}
			if( good )
				filteredUnique.push_back( filtered[ k ]->pt );
		}

		KLTPType::extractPatches( _patches, filteredUnique, img );
	}
			
}
