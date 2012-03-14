#ifndef FEATURE_GUI_H
#define FEATURE_GUI_H

#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/Slider.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/Label.h>
#include <cvt/gfx/GFXEngineImage.h>

#include <list>

namespace cvt
{

	class Gui : public Window
	{
		public:
			Gui();

			void updateImage( const cvt::Image & img,
                              const std::vector<Feature2Df> & features );

			void observeFastThresholdSlider( const Delegate<void ( uint8_t )> & delegate );
            void observeOctaveSlider( const Delegate<void ( size_t )> & delegate );
            void observeScaleSlider( const Delegate<void ( float )> & delegate );
            void observeMaxFeatureSlider( const Delegate<void ( size_t )> & delegate );
            void observeNonMaxSuppression( const Delegate<void (ToggleButton *)> & delegate );

            void setFPS( float fps );

		private:
			ImageView		_imgView;
			Moveable		_imgMov;
			Button			_quit;
			Slider<uint8_t> _fastThreshSlider;
            Label           _fastThreshLabel;

            Slider<size_t>  _octaveSlider;
            Label           _octaveLabel;

            Slider<float>   _scaleSlider;
            Label           _scaleLabel;

            Slider<size_t>  _maxFeatureSlider;
            Label           _maxFeatureLabel;

            ToggleButton    _nonMaxSuppression;

            Label           _fpsLabel;

            void updateFastSliderLabel( uint8_t thresh );
            void updateOctaveLabel( size_t v );
            void updateScaleLabel( float v );
            void updateMaxFeatureLabel( size_t v );
	};

	inline Gui::Gui():
		Window( "Feature Tracking GUI" ),
		_imgView(),
		_imgMov( &_imgView ),
		_quit( "Quit" ),
		_fastThreshSlider( 10, 120, 25 ),
        _fastThreshLabel( "FAST threshold:" ),
        _octaveSlider( 1, 5, 3 ),
        _octaveLabel( "Octaves: " ),
        _scaleSlider( 0.3f, 0.9f, 0.6f ),
        _scaleLabel( "Scale Factor: " ),
        _maxFeatureSlider( 50, 3000, 1000 ),
        _maxFeatureLabel( "Max Features: " ),
        _nonMaxSuppression( "Non Max Suppression", false ),
        _fpsLabel( "FPS: 0.0f" )
	{
		setSize( 960, 720 );

		_imgMov.setSize( 640, 480 );
		addWidget( &_imgMov );

		Delegate<void ()> qDel( &Application::exit );
		_quit.clicked.add( qDel );

        Delegate<void ( uint8_t )> threshChange( this, &Gui::updateFastSliderLabel );
        observeFastThresholdSlider( threshChange );

        Delegate<void ( size_t )> octaveChange( this, &Gui::updateOctaveLabel );
        observeOctaveSlider( octaveChange );

        Delegate<void ( float )> scaleChange( this, &Gui::updateScaleLabel );
        observeScaleSlider( scaleChange );

        Delegate<void ( size_t )> maxFeatChange( this, &Gui::updateMaxFeatureLabel );
        observeMaxFeatureSlider( maxFeatChange );

        WidgetLayout wl;
        size_t h = 10;
		wl.setAnchoredRight( 10, 100 );
		wl.setAnchoredBottom( h, 20 );
		addWidget( &_quit, wl );

        h += 30;
        wl.setAnchoredRight( 10, 150 );
		wl.setAnchoredBottom( h, 20 );
		addWidget( &_fastThreshSlider, wl );
        wl.setAnchoredRight( 160, 150 );
        addWidget( &_fastThreshLabel, wl );
        updateFastSliderLabel( 25 );

        h+=30;
        wl.setAnchoredRight( 10, 150 );
		wl.setAnchoredBottom( h, 20 );
		addWidget( &_octaveSlider, wl );
        wl.setAnchoredRight( 160, 150 );
        addWidget( &_octaveLabel, wl );
        updateOctaveLabel( 3 );

        h+=30;
        wl.setAnchoredRight( 10, 150 );
		wl.setAnchoredBottom( h, 20 );
		addWidget( &_scaleSlider, wl );
        wl.setAnchoredRight( 160, 150 );
        addWidget( &_scaleLabel, wl );
        updateScaleLabel( 0.6f );

        h+=30;
        wl.setAnchoredRight( 10, 150 );
		wl.setAnchoredBottom( h, 20 );
		addWidget( &_maxFeatureSlider, wl );
        wl.setAnchoredRight( 160, 150 );
        addWidget( &_maxFeatureLabel, wl );
        updateMaxFeatureLabel( 1000 );

        h+=30;
        wl.setAnchoredRight( 10, 300 );
		wl.setAnchoredBottom( h, 20 );
		addWidget( &_nonMaxSuppression, wl );

        h+=30;
        wl.setAnchoredRight( 160, 150 );
		wl.setAnchoredBottom( h, 20 );
		addWidget( &_fpsLabel, wl );

		setVisible( true );
	}

	inline void Gui::observeFastThresholdSlider( const Delegate<void ( uint8_t )> & delegate )
	{
	   	_fastThreshSlider.valueChanged.add( delegate );
	}

    inline void Gui::observeOctaveSlider( const Delegate<void ( size_t )> & delegate )
	{
	   	_octaveSlider.valueChanged.add( delegate );
	}

    inline void Gui::observeScaleSlider( const Delegate<void ( float )> & delegate )
    {
        _scaleSlider.valueChanged.add( delegate );
    }

    inline void Gui::observeMaxFeatureSlider( const Delegate<void ( size_t )> & delegate )
    {
        _maxFeatureSlider.valueChanged.add( delegate );
    }

    inline void Gui::observeNonMaxSuppression( const Delegate<void (ToggleButton *)> & delegate )
    {
        _nonMaxSuppression.toggled.add( delegate );
    }

    inline void Gui::updateFastSliderLabel( uint8_t thresh )
    {
        String title( "FAST threshold: " );
        title += thresh;
        _fastThreshLabel.setLabel( title );
    }

    inline void Gui::updateOctaveLabel( size_t v )
    {
        String title( "Octaves: " );
        title += v;
        _octaveLabel.setLabel( title );
    }

    inline void Gui::updateScaleLabel( float v )
    {
        String title;
        title.sprintf( "Scale Factor: %0.2f", v );
        _scaleLabel.setLabel( title );
    }

    inline void Gui::updateMaxFeatureLabel( size_t v )
    {
        String title( "Max Features: " );
        title += v;
        _maxFeatureLabel.setLabel( title );
    }

    inline void Gui::setFPS( float fps )
    {
        String title;
        title.sprintf( "FPS: %0.2f", fps );
        _fpsLabel.setLabel( title );
    }

	inline void Gui::updateImage( const cvt::Image & img,
                                  const std::vector<Feature2Df> & features )
	{
        Image out( img );
        {
            GFXEngineImage ge( out );
            GFX g( &ge );
            
            g.color() = Color::GREEN;
            std::vector<Feature2Df>::const_iterator tIter = features.begin();
            std::vector<Feature2Df>::const_iterator tEnd = features.end();
            while( tIter != tEnd ){
                g.fillRect( tIter->pt.x-1, tIter->pt.y-1, 3, 3 );
                tIter++;
            }
		}

		_imgView.setImage( out );
	}
}


#endif
