#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Label.h>
#include <cvt/gui/Slider.h>
#include <cvt/util/Delegate.h>

#include "RectSelectImageView.h"

namespace cvt
{

    class MIGui : public Window
    {
        public:
            MIGui();
            ~MIGui();

            void setTemplateImage( const Image& img,
                                   const Image& gradX,
                                   const Image& gradY );

            void setInputImage( const Image& img );

            void setFPS( double fps );

            void setMaxIter( size_t maxIter );
            void observeMaxIterations( const Delegate<void (size_t)> & d );

            void resizeEvent( ResizeEvent* e );


            /* register Delegate for the change of the rectangle */
            void addSelectionDelegates( const Delegate<void( void )>& selStartD,
                                        const Delegate<void( const Rectf& )>& selEndD );

            void setPoints( const std::vector<Vector2f> & pts );

        private:
            void setIterLabel( size_t );

            RectSelectImageView _inputView;

            ImageView		_templateView;
            ImageView		_templateGradX;
            ImageView		_templateGradY;
            float			_templateAspect;
            float			_inputAspect;
            Label			_fpsLabel;
            Label			_iterLabel;
            Slider<size_t>	_iterSlider;

            void initGuiElements();

            void resizeImageViews();
    };


    inline MIGui::MIGui() : Window( "Mutual Information Template Tracking" ),
        _templateAspect( 1.5f ),
        _inputAspect( 1.5f ),
        _fpsLabel( "" ),
        _iterLabel( "" ),
        _iterSlider( 1, 70, 1 )
    {
        initGuiElements();
    }

    inline MIGui::~MIGui()
    {
    }

    inline void MIGui::initGuiElements()
    {
        setSize( 800, 600 );
        setPosition( 100, 100 );

        _fpsLabel.color() = Color::GREEN;

        // add the template views:
        resizeImageViews();

        addWidget( &_templateView );
        addWidget( &_templateGradX );
        addWidget( &_templateGradY );
        addWidget( &_inputView );
        addWidget( &_fpsLabel );
        addWidget( &_iterSlider );
        addWidget( &_iterLabel );

        setVisible( true );

        _iterSlider.valueChanged.add( Delegate<void (size_t)>( this, &MIGui::setIterLabel ) );
    }

    inline void MIGui::setTemplateImage( const Image& img,
                                         const Image& gradX,
                                         const Image& gradY )
    {
        _templateAspect = ( float )img.width() / ( float )img.height();
        _templateView.setImage( img );
        _templateGradX.setImage( gradX );
        _templateGradY.setImage( gradY );
        resizeImageViews();
    }

    inline void MIGui::setInputImage( const Image& img )
    {
        _inputAspect = ( float )img.width() / ( float )img.height();
        _inputView.setImage( img );
    }

    inline void MIGui::resizeImageViews()
    {
        int width, height;
        size( width, height );
        float h = height / 3.0f;
        float w = _templateAspect * h;
        _templateView.setSize( w, h );
        _templateGradX.setSize( w, h );
        _templateGradY.setSize( w, h );

        size_t currH = h;
        _templateGradX.setPosition( 0, currH );
        currH += h;
        _templateGradY.setPosition( 0, currH );

        _fpsLabel.setPosition( (size_t)w + 10, 10 );
        size_t wHalf = ( width - w - 20 ) / 2;
        _fpsLabel.setSize( wHalf * 0.5, 20 );
        const Recti& fpsRect = _fpsLabel.rect();

        _iterLabel.setPosition( fpsRect.x +  wHalf * 0.5f, fpsRect.y );
        _iterLabel.setSize( wHalf * 0.5f, 20 );

        _iterSlider.setPosition( fpsRect.x + wHalf, fpsRect.y );
        _iterSlider.setSize( wHalf, 20 );

        _inputView.setPosition( fpsRect.x, fpsRect.y + fpsRect.height + 20 );
        float iW =  width - w - 20;
        float iH = iW / _inputAspect;
        _inputView.setSize( iW, iH );
    }

    inline void MIGui::resizeEvent( ResizeEvent* e )
    {
        Window::resizeEvent( e );
        resizeImageViews();
    }

    inline void MIGui::addSelectionDelegates( const Delegate<void (void)> & dStart,
                                              const Delegate<void (const Rectf&)> & dDone )
    {
        _inputView.selectionDidStart.add( dStart );
        _inputView.selectionComplete.add( dDone );
    }

    inline void MIGui::setFPS( double fps )
    {
        String s;
        s.sprintf( "FPS: %0.1f", fps );
        _fpsLabel.setLabel( s );
    }

    inline void MIGui::setIterLabel( size_t maxIter )
    {
        String s;
        s.sprintf( "Iter: %zd", maxIter );
        _iterLabel.setLabel( s );
    }

    inline void MIGui::setMaxIter( size_t maxIter )
    {
        _iterSlider.setValue( maxIter );
    }

    inline void MIGui::observeMaxIterations( const Delegate<void (size_t)> & d )
    {
        _iterSlider.valueChanged.add( d );
    }

    inline void MIGui::setPoints( const std::vector<Vector2f> & pts )
    {
        _inputView.updatePoints( pts );
    }


}
