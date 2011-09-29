#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>

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

			void resizeEvent( ResizeEvent* e );
		private:
			ImageView	_templateView;
			ImageView	_templateGradX;
			ImageView	_templateGradY;
			float		_templateAspect;

			void initGuiElements();

			void resizeImageViews();
	};


	inline MIGui::MIGui() : Window( "Mutual Information Template Tracking" ),
		_templateAspect( 1.5f )
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

		// add the template views:
		resizeImageViews();

		addWidget( &_templateView );
		addWidget( &_templateGradX );
		addWidget( &_templateGradY );

		setVisible( true );
	}
			
	inline void MIGui::setTemplateImage( const Image& img, 
										 const Image& gradX,
										 const Image& gradY )
	{
		_templateAspect = ( float )img.width() / ( float )img.height();
		_templateView.setImage( img );
		_templateGradX.setImage( gradX );
		_templateGradY.setImage( gradY );
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
	}

	inline void MIGui::resizeEvent( ResizeEvent* e )
	{
		Window::resizeEvent( e );
		resizeImageViews();
	}

}
