#include "KLTWindow.h"

namespace cvt
{
    KLTWindow::KLTWindow( VideoInput & video ) :
        _window( "KLT" ),
        _ssdSliderLabel( "SSD Threshold" ),
        _ssdSlider( 0.0f, 255.0f, 20.0f ),
        _stepButton( "Enable Freerun" ),
        _nextButton( "Next Frame" ),
        _stepping( true ),
        _next( false ),
        _video( video ),
        _kltTimeSum( 0.0 ),
        _kltIters( 0 ),
        _pyramid( 1, 0.5f ),
        _redetectThreshold( 20 ),
        _gridFilter( 40, video.width(), video.height() )
    {
        _ssdThresh = Math::sqr( 20 );

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

        _video.nextFrame();
        _video.nextFrame();
        _video.nextFrame();
        _video.nextFrame();
        _video.nextFrame();
        _video.nextFrame();

        Image gray( _video.width(), _video.height(), IFormat::GRAY_UINT8 );
        _video.frame().convert( gray, IFormat::GRAY_UINT8 );

        _pyramid.update( gray );

        _fast.setThreshold( 20 );
        _fast.setNonMaxSuppress( true );
        _fast.setBorder( PATCH_SIZE / 2 + 1 );

        redetectMultiScale();
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
        _ssdThresh = Math::sqr( val );
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

    size_t KLTWindow::track()
    {
        std::vector<PatchType*> lost;
        std::vector<PatchType*> tracked;

        SIMD* simd = SIMD::instance();
        const size_t nPixels = Math::sqr( PATCH_SIZE );
        const float maxSSD = nPixels * _ssdThresh;

        // track all patches
        for( size_t i = 0; i < _patches.size(); i++ ){
            if( _patches[ i ]->alignWithImage( _pyramid[ 0 ] ) ){
                float ssd = simd->SSD( _patches[ i ]->pixels(), _patches[ i ]->transformed(), nPixels );
                if( ssd < maxSSD )
                    tracked.push_back( _patches[ i ] );
                else
                    lost.push_back( _patches[ i ] );
            } else {
                lost.push_back( _patches[ i ] );
            }
        }

        for( size_t i = 0; i < lost.size(); i++ ){
            delete lost[ i ];
        }
        _patches = tracked;
        return lost.size();
    }

    void KLTWindow::onTimeout()
    {
        if( _stepping && !_next )
            return;
        _next = false;
        // convert to grayscale
        Image gray( _video.width(), _video.height(), IFormat::GRAY_UINT8 );
        _video.frame().convert( gray, IFormat::GRAY_UINT8 );
        _pyramid.update( gray );

        _kltTime.reset();

        //size_t lost = trackSingleScale( _pyramid[ 0 ] );
        size_t lost = track();

        //createPatchImage( _patches );
        //_patchImage.save( "featurepatches.png" );

        size_t numAll = _patches.size() + lost;

        _kltTimeSum += _kltTime.elapsedMilliSeconds();
        _kltIters++;

        size_t numTracked = _patches.size();

        if( _patches.size() < _redetectThreshold ){
            //redetectFeatures( _pyramid[ 0 ] );
            redetectMultiScale();
        }

        double t = _time.elapsedSeconds();
        _iter++;
        if( t > 3.0 ){
            _fps = _iter / t;
            _time.reset();
            _iter = 0;
        }

        String title;
        title.sprintf( "KLT: tracked=%d, all=%d, fps=%0.2f, avgklt=%0.2fms", numTracked, numAll, _fps, _kltTimeSum/_kltIters );
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

    void KLTWindow::redetectMultiScale()
    {
        std::vector<Feature2Df> features;
        float scale = 1.0f;
        for( size_t i = 0; i < _pyramid.octaves(); i++ ){
            std::vector<Feature2Df> scaleFeatures;
            VectorFeature2DInserter<float> inserter( scaleFeatures );
            _fast.extract( _pyramid[ i ], inserter );

            size_t start = features.size();
            // insert the features
            features.insert( features.end(), scaleFeatures.begin(), scaleFeatures.end() );
            if( i != 0 ){

                scale /= _pyramid.scaleFactor();
                for( size_t f = start; f < features.size(); f++ ){
                    features[ f ].pt *= scale;
                }
            }
        }

        _gridFilter.clear();
        for( size_t i = 0; i < features.size(); i++ ){
            _gridFilter.addFeature( &features[ i ] );
        }

        std::vector<const Feature2Df*> filtered;
        _gridFilter.gridFilteredFeatures( filtered, 500 );

        std::vector<Vector2f> filteredUnique;

        std::vector<Vector2f> patchPositions;
        patchPositions.resize( _patches.size() );
        for( size_t i = 0; i < _patches.size(); i++ ){
            _patches[ i ]->currentCenter( patchPositions[ i ] );
        }

        IMapScoped<const uint8_t> map( _pyramid[ 0 ] );
        for( size_t k = 0; k < filtered.size(); k++ ){
            bool good = true;
            for( size_t i = 0; i < patchPositions.size(); i++ ){
                if( ( patchPositions[ i ] - filtered[ k ]->pt ).lengthSqr() < 100.0f ){
                    good = false;
                    break;
                }
            }
            if( good ){
                filteredUnique.push_back( filtered[ k ]->pt );
                _patches.push_back( new PatchType() );
                _patches.back()->update( map.ptr(), map.stride(), filtered[ k ]->pt );
            }
        }


    }

    void KLTWindow::createPatchImage( const std::vector<PatchType*>& patches )
    {
        // according to the number of
        //patches: say we have an image with 1024:
        size_t spacePerPatch = PATCH_SIZE + 5;
        size_t heightPerPatch = 2 * PATCH_SIZE + 5;
        size_t numPatchesPerLine = 640.0f / spacePerPatch;
        size_t numLines = 1 + patches.size() / numPatchesPerLine;
        _patchImage.reallocate( 640, numLines * heightPerPatch, IFormat::GRAY_UINT8 );

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

    void KLTWindow::dumpPatch( PatchType& patch, size_t i )
    {
        // evaluate some cost functions:
        SIMD* simd = SIMD::instance();

        size_t sad = simd->SAD( patch.pixels(), patch.transformed(), Math::sqr( PATCH_SIZE ) );
        float  ssd = simd->SSD( patch.pixels(), patch.transformed(), Math::sqr( PATCH_SIZE ) );


        if( sad > ( size_t )( 30 * Math::sqr( PATCH_SIZE ) ) ) {
            Image out( PATCH_SIZE * 2, PATCH_SIZE, IFormat::GRAY_UINT8 );

            {
                IMapScoped<uint8_t> map( out );
                const uint8_t* p = patch.pixels();
                const uint8_t* t = patch.transformed();

                size_t h = PATCH_SIZE;
                while( h-- ){
                    uint8_t* pout = map.ptr();

                    for( size_t i = 0; i < PATCH_SIZE; i++ ){
                        *pout++ = *p++;
                    }
                    for( size_t i = 0; i < PATCH_SIZE; i++ ){
                        *pout++ = *t++;
                    }
                    map++;
                }
            }

            String filename;
            filename.sprintf( "patch_%02d_sad_%02d_ssd_%0.1f.png", i, sad, ssd );
            out.save( filename );
        }

    }

}
