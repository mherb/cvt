#ifndef CVT_RGBDDATASET_APP_H
#define CVT_RGBDDATASET_APP_H

#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/ImageView.h>

#include "RGBDParser.h"

namespace cvt
{
	class RGBDDatasetApp : public TimeoutHandler
	{
		public:
			RGBDDatasetApp( const String& datasetFolder ) :
				_window( "RGBD Dataset" ),
				_rgbMov( &_rgbView ),
				_depthMov( &_depthView ),
				_rgbdset( datasetFolder ),
				_loadNext( true )
			{
				_window.setSize( 800, 600 );
				_window.setVisible( true );
				_window.setMinimumSize( 320, 240 );


				_rgbMov.setTitle( "RGB" );
				_depthMov.setTitle( "Depth" );
				_rgbMov.setSize( 320, 240 );
				_depthMov.setSize( 320, 240 );
				_window.addWidget( &_rgbMov );
				_window.addWidget( &_depthMov );
				
				_timerId = Application::registerTimer( 20, this );
			}

			~RGBDDatasetApp()
			{
			}
			
			void onTimeout()
			{
				if( _loadNext ){
					_rgbdset.loadNext();
					const RGBDParser::RGBDSample& sample = _rgbdset.data();

					_rgbView.setImage( sample.rgb );
					_depthView.setImage( sample.depth );

					String title;
					title.sprintf( "RGBD Dataset: %d / %d", _rgbdset.iter(), _rgbdset.size() );
					_window.setTitle( title );
				}
			}

		private:
			Window		_window;
			ImageView	_rgbView;
			ImageView	_depthView;
			Moveable	_rgbMov;
			Moveable	_depthMov;
			
			RGBDParser	_rgbdset;
			uint32_t	_timerId;
			bool		_loadNext;

	};
}

#endif
