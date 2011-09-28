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

		private:
			ImageView	_templateView;
			ImageView	_templateGradX;
			ImageView	_templateGradY;

			void initGuiElements();
	};


	inline MIGui::MIGui() : Window( "Mutual Information Template Tracking" )
	{
		initGuiElements();
	}

	inline MIGui::~MIGui()
	{
	}

	inline void MIGui::initGuiElements()
	{
		setSize( 800, 600 );

		// add the template views:
		_templateView.setSize( 300, 200 );
		_templateGradX.setPosition( 0, 200 );
		_templateGradX.setSize( 300, 200 );
		_templateGradY.setPosition( 0, 400 );
		_templateGradY.setSize( 300, 200 );

		addWidget( &_templateView );
		addWidget( &_templateGradX );
		addWidget( &_templateGradY );

		setVisible( true );
	}
			
	inline void MIGui::setTemplateImage( const Image& img, 
										 const Image& gradX,
										 const Image& gradY )
	{
		_templateView.setImage( img );
		_templateGradX.setImage( gradX );
		_templateGradY.setImage( gradY );
	}

}
