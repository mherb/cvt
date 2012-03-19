#include "KLTWindow.h"
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/Delegate.h>

namespace cvt
{

	KLTWindow::KLTWindow( Image & img ) :
		_window( "KLT" ),	
		_klt( 1 ),
		_kltTimeSum( 0.0 ),
		_kltIters( 0 ),
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

		_pyramid.resize( 3 );
		img.convert( _pyramid[ 0 ], IFormat::GRAY_UINT8 );
		createPyramid();
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
			std::vector<size_t>	  trackedIndices;
			_kltTime.reset();

			IMapScoped<const uint8_t> map( _pyramid[ 0 ] );
			_klt.trackPatch( *_patches[ 0 ], 
							map.ptr(), map.stride(),
							_pyramid[ 0 ].width(), _pyramid[ 0 ].height() );

			std::cout << _patches[ 0 ]->pose().transformation() << std::endl;

			_kltTimeSum += _kltTime.elapsedMilliSeconds();
			_kltIters++;

			double t = _time.elapsedSeconds();
			_iter++;
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

	void KLTWindow::initialize()
	{
		// we take a patch in the middle
		KLTPType* p = new KLTPType();

		Vector2f initPos;
		initPos.x = _pyramid[ 0 ].width() / 2;
		initPos.y = _pyramid[ 0 ].height() / 2;
		
		size_t stride;
		const uint8_t* ptr = _pyramid[ 0 ].map( &stride );
		p->update( ptr, stride, initPos );

		_pyramid[ 0 ].unmap( ptr );

		_patches.push_back( p );

		Eigen::Matrix3f curr = _patches[ 0 ]->pose().transformation();

		std::cout << "curr: " << curr << std::endl;

		Matrix3f affine;
		affine.setAffine( Math::deg2Rad( 10.0f ), 0, 1.1f, 1.0f, curr( 0, 2 ), curr( 1, 2 ) );
		_patches[ 0 ]->pose().set( affine );

		std::cout << "After update: " << _patches[ 0 ]->pose().transformation() << std::endl;
	}
	
	void KLTWindow::drawRect( Image & img )
	{
		{
			GFXEngineImage ge( img );
			GFX g( &ge );
			g.color() = Color::GREEN;
	
			Eigen::Vector2f p0, p1, p2, p3;
			Eigen::Vector2f a, b, c, d;


			size_t psize = _patches[ 0 ]->size();
			p0[ 0 ] = 0.0f;	 p0[ 1 ] = 0.0f;
			p1[ 0 ] = psize; p1[ 1 ] = 0.0f; 
			p2[ 0 ] = psize; p2[ 1 ] = psize;
			p3[ 0 ] = 0.0f;  p3[ 1 ] = psize;

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

	void KLTWindow::createPyramid()
	{
		size_t w = _pyramid[ 0 ].width();
		size_t h = _pyramid[ 0 ].height();

		IScaleFilterBilinear filter;
		float scale = 0.7f;
		for( size_t i = 1; i < _pyramid.size(); i++ ){
			w *= scale;
			h *= scale;
			_pyramid[ i - 1 ].scale( _pyramid[ i ], w, h, filter );
		}
	}
}
