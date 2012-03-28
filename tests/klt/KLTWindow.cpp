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
		_klt( 12 ),
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

		size_t numAll = 0;
		{
			// map the image
			IMapScoped<uint8_t> map( _pyramid[ 0 ] );
			size_t w = _pyramid[ 0 ].width();
			size_t h = _pyramid[ 0 ].height();

			std::vector<KLTPType*> lost;
			std::vector<KLTPType*> tracked;

			// track all patches
			for( size_t i = 0; i < _patches.size(); i++ ){
				if( _klt.trackPatch( *_patches[ i ], map.ptr(), map.stride(), w, h ) ){
						tracked.push_back( _patches[ i ] );
				} else {
					lost.push_back( _patches[ i ] );
				}
			}
			numAll = _patches.size();

			createPatchImage( lost );
			_patchImage.save( "lostfeatures.png" );
			SIMD* simd = SIMD::instance();
			float psqr = Math::sqr( PATCH_SIZE );
			for( size_t i = 0; i < lost.size(); i++ ){
				size_t sad = simd->SAD( lost[ i ]->pixels(), lost[ i ]->transformed(), Math::sqr( PATCH_SIZE ) );
				size_t ssd = simd->SSD( lost[ i ]->pixels(), lost[ i ]->transformed(), Math::sqr( PATCH_SIZE ) );

				std::cout << "LOST PATCH: ssd=" << ssd / psqr << ", sad: " << sad / psqr << std::endl;

				delete lost[ i ];
			}
			_patches = tracked;
		}


		createPatchImage( _patches );
		_patchImage.save( "featurepatches.png" );

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
		title.sprintf( "KLT: tracked=%d, all=%d, fps=%0.2f, avgklt=%0.2fms", _patches.size(), numAll, _fps, _kltTimeSum/_kltIters );
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
		_gridFilter.gridFilteredFeatures( filtered, 400 );

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

	void KLTWindow::createPatchImage( const std::vector<KLTPType*>& patches )
	{
		// according to the number of 
		//patches: say we have an image with 1024:
		size_t spacePerPatch = PATCH_SIZE + 5;
		size_t heightPerPatch = 2 * PATCH_SIZE + 5;
		size_t numPatchesPerLine = 1024.0f / spacePerPatch;
		size_t numLines = 1 + patches.size() / numPatchesPerLine;
		_patchImage.reallocate( 1024, numLines * heightPerPatch, IFormat::GRAY_UINT8 );

		_patchImage.fill( Color::BLACK );

		IMapScoped<uint8_t> map( _patchImage );
		uint8_t* ptr = map.ptr();
		size_t currentXPos = 0;
		SIMD * simd = SIMD::instance();
		
		for( size_t i = 0; i < patches.size(); i++ ){
			uint8_t * p = ptr + currentXPos * spacePerPatch;	
			const uint8_t * patch = patches[ i ]->pixels();
			size_t n = PATCH_SIZE;
			while( n-- ){
				simd->Memcpy( p, patch, PATCH_SIZE );
				p += map.stride();
				patch += PATCH_SIZE;
			}
			n = PATCH_SIZE;
			patch = patches[ i ]->transformed();
			while( n-- ){
				simd->Memcpy( p, patch, PATCH_SIZE );
				p += map.stride();
				patch += PATCH_SIZE;
			}

			// set the pointer:
			currentXPos++;
			if( currentXPos == numPatchesPerLine ){
				ptr += map.stride() * heightPerPatch;	
				currentXPos = 0;
			} 
		}
	}
			
}
