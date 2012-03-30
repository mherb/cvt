#include "KLTWindow.h"
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/Delegate.h>

//#define MULTISCALE

namespace cvt
{

	KLTWindow::KLTWindow( Image & img ) :
		_window( "KLT" ),	
		_klt( 1 ),
		_kltTimeSum( 0.0 ),
		_kltIters( 0 ),
		_pyramid( 3, 0.5f ),
		_stepButton( "Step" ),
		_step( false )
	{
		_timerId = Application::registerTimer( 10, this );
		_window.setSize( 640, 480 );
		WidgetLayout wl;
		wl.setRelativeTopBottom( 0.01f, 0.9f );
		wl.setRelativeLeftRight( 0.01f, 0.99f );
		_window.addWidget( &_imView, wl );
		
		wl.setRelativeTopBottom( 0.91f, 0.99f );
		_window.addWidget( &_stepButton, wl );

		_window.setVisible( true );

		Image gray;
		img.convert( gray, IFormat::GRAY_UINT8 );

		_pyramid.update( gray );
		
		_fps = 0.0;
		_iter = 0;

		initialize();

		Image debug;
		_pyramid[ 0 ].convert( debug, IFormat::RGBA_UINT8 );
		drawRect( debug );
		_imView.setImage( debug );

		_window.update();

		Delegate<void ()> d( this, &KLTWindow::nextStep );
		_stepButton.clicked.add( d );
	}

	KLTWindow::~KLTWindow()
	{
		Application::unregisterTimer( _timerId );
	}


	void KLTWindow::onTimeout()
	{
		if( _step ){
			_kltTime.reset();

#ifdef MULTISCALE
			multiScaleUpdate();
#else
			singleScaleUpdate();
#endif

			Matrix3f curr;
			EigenBridge::toCVT( curr, _patches[ 0 ]->pose().transformation() );

			_iter++;
			std::cout << _iter << ": Error\n" << _groundtruth - curr << std::endl;

			_kltTimeSum += _kltTime.elapsedMilliSeconds();
			_kltIters++;

			double t = _time.elapsedSeconds();
			if( t > 3.0 ){
				_fps = _iter / t;
				_time.reset();
				_iter = 0;
			}

			String title;
			title.sprintf( "KLT: fps=%0.2f, avgklt=%0.2fms", _fps, _kltTimeSum/_kltIters );
			_window.setTitle( title );

			Image debug;
			_pyramid[ 0 ].convert( debug, IFormat::RGBA_UINT8 );
			drawRect( debug );
			_imView.setImage( debug );
			_step = false;
		}
	}

	void KLTWindow::singleScaleUpdate()
	{
		IMapScoped<const uint8_t> map( _pyramid[ 0 ] );
 		bool res = _klt.trackPatch( *_patches[ 0 ], 
									map.ptr(), map.stride(),
									_pyramid[ 0 ].width(),
									_pyramid[ 0 ].height() );
		if( !res ){
			std::cout << "LOST PATCH" << std::endl;
		}
	}

	void KLTWindow::multiScaleUpdate()
	{
		bool res = _klt.trackPatchMultiscale( *_patches[ 0 ], _pyramid );

		if( !res ){
			std::cout << "LOST PATCH" << std::endl;
		}
						  
	}

	void KLTWindow::initialize()
	{
		// we take a patch in the middle
		Vector2f initPos;
		initPos.x = _pyramid[ 0 ].width() / 2;
		initPos.y = _pyramid[ 0 ].height() / 2;

		std::vector<Vector2f> positions;
		positions.push_back( initPos );

#ifdef MULTISCALE
		KLTPType::extractPatches( _patches, positions, _pyramid );
		savePatchAsImage( "patch.png" );
#else
		KLTPType::extractPatches( _patches, positions, _pyramid[ 0 ] );
#endif

		if( _patches.size() == 0 ){
			throw CVTException( "PATCH NOT BUILD" );
		}
		

		Eigen::Matrix3f curr = _patches[ 0 ]->pose().transformation();
		EigenBridge::toCVT( _groundtruth, curr );

		// displace it
		Matrix3f affine;
		affine.setIdentity();
		//affine[ 0 ][ 2 ] = curr( 0, 2 ) - 15;
		//affine[ 1 ][ 2 ] = curr( 1, 2 ) - 15;
		affine.setAffine( Math::deg2Rad( 10.0f ), 0, 1.1f, 1.0f, curr( 0, 2 ), curr( 1, 2 ) );
		_patches[ 0 ]->pose().set( affine );

	}
	
	void KLTWindow::drawRect( Image & img )
	{
		{
			GFXEngineImage ge( img );
			GFX g( &ge );
			g.color() = Color::GREEN;
	
			Eigen::Vector2f p0, p1, p2, p3;
			Eigen::Vector2f a, b, c, d;


			size_t half = _patches[ 0 ]->size();
			p0[ 0 ] = -half; p0[ 1 ] = -half;
			p1[ 0 ] =  half; p1[ 1 ] = -half; 
			p2[ 0 ] =  half; p2[ 1 ] =  half;
			p3[ 0 ] = -half; p3[ 1 ] =  half;

			const PoseType& pose = _patches[ 0 ]->pose();
			pose.transform( a, p0 );
			pose.transform( b, p1 );
			pose.transform( c, p2 );
			pose.transform( d, p3 );

			Vector2f ca, cb, cc, cd;
			EigenBridge::toCVT( ca, a );
			EigenBridge::toCVT( cb, b );
			EigenBridge::toCVT( cc, c );
			EigenBridge::toCVT( cd, d );

			g.drawLine( ca, cb );
			g.drawLine( cb, cc );
			g.drawLine( cc, cd );
			g.drawLine( cd, ca );

		}
	}

			
	void KLTWindow::savePatchAsImage( const String& filename )
	{
		KLTPType & patch = *_patches[ 0 ];

		size_t octaves = _pyramid.octaves();

		Image patchImage( octaves * patch.size(), patch.size(), IFormat::GRAY_UINT8 );

		IMapScoped<uint8_t> map( patchImage );

		for( size_t h = 0; h < patchImage.height(); h++ ){
			uint8_t* line = map.ptr();
			for( size_t o = 0; o < octaves; o++ ){
				for( size_t x = 0; x < patch.size(); x++ ){
					*line++ = patch.pixels( o )[ h * patch.size() + x ];
				}				
			}
			map++;
		}

		patchImage.save( filename );
	}
}
