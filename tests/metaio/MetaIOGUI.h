#ifndef CVT_METAIO_GUI_H
#define CVT_METAIO_GUI_H

#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/gfx/GFX.h>

namespace cvt
{
	class MetaioGUI : public Window, TimeoutHandler
	{
		public:
			MetaioGUI( MetaioData& data ) : Window( "MetaIO Dataset" ),
				_data( data )
			{
				_template.setSize( 400, 300 );
				_current.setSize( 400, 300 );
				_current.setPosition( 400, 0 );
				_template.setImage( _data.templateImage() );
				this->addWidget( &_template );
				this->addWidget( &_current );
				setSize( 800, 600 );
				setVisible( true );

				// register a timer for the runloop:
				_timerId = Application::registerTimer( 10, this );
			}

			~MetaioGUI()
			{
				Application::unregisterTimer( _timerId );
			}

			void onTimeout()
			{
				// process the next image of the dataset
				// calculate the homography from the feature matches
				// draw the resulting homography
				std::vector<Vector2d> gtPts;
				Matrix3d homography;

				bool drawGT = _data.hasGroundTruthForFrame( gtPts );

				Image current = _data.processNext( homography );

				// draw on the image
				if( drawGT ){
					drawRectangleInImage( current, gtPts );
				}

				_current.setImage( current );


				if( _data.finished() )
					Application::exit();	
			}

		private:
			MetaioData &	_data;
			ImageView		_template;
			ImageView		_current;
			uint32_t		_timerId;

			void drawRectangleInImage( Image & img, const std::vector<Vector2d> & pts ) const
			{
				GFXEngineImage ge( img );
				{
					GFX gfx( &ge );
					gfx.color() = Color::GREEN;
					gfx.setLineWidth( 3 );
					Vector2f p0, p1;

					float w2 = 0.0f;
					float h2 = 0.0f;

					p0.x = pts[ 0 ].x + w2;	p0.y = pts[ 0 ].y + h2;
					p1.x = pts[ 1 ].x + w2;	p1.y = pts[ 1 ].y + h2;
					gfx.drawLine( p0, p1 );
					p0.x = pts[ 2 ].x + w2;	p0.y = pts[ 2 ].y + h2;
					gfx.drawLine( p0, p1 );
					p1.x = pts[ 3 ].x + w2;	p1.y = pts[ 3 ].y + h2;
					gfx.drawLine( p0, p1 );
					p0.x = pts[ 0 ].x + w2;	p0.y = pts[ 0 ].y + h2;
					gfx.drawLine( p0, p1 );
				}
			}	

	};

}

#endif
