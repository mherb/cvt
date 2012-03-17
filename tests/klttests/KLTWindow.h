#ifndef CVT_KLT_WINDOW_H
#define CVT_KLT_WINDOW_H

#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>

#include <cvt/util/Time.h>
#include <cvt/util/EigenBridge.h>

#include <cvt/math/Translation2D.h>
#include <cvt/math/Sim2.h>
#include <cvt/math/GA2.h>

#include <cvt/vision/KLTTracker.h>

#include <cvt/gfx/GFXEngineImage.h>

namespace cvt {

	class KLTWindow : public TimeoutHandler
	{
		public:
			typedef GA2<float>					PoseType;
			//typedef Sim2<float>				PoseType;
			//typedef Translation2D<float>		PoseType;
			typedef KLTTracker<PoseType, 32>	KLTType;
			typedef KLTType::KLTPType			KLTPType;

			KLTWindow( Image & img ) :
				_window( "KLT" ),	
				_klt( 1 ),
				_kltTimeSum( 0.0 ),
				_kltIters( 0 )
			{
				_timerId = Application::registerTimer( 10, this );
				_window.setSize( 640, 480 );
				WidgetLayout wl;
				wl.setAnchoredTopBottom( 0, 0 );
				wl.setAnchoredLeftRight( 0, 0 );
				_window.addWidget( &_imView, wl );
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
			}

			~KLTWindow()
			{
				Application::unregisterTimer( _timerId );
			}

			void onTimeout();

		private:
			uint32_t				_timerId;
			Window					_window;
			ImageView				_imView;

			KLTType					_klt;
			Time					_kltTime;
			double					_kltTimeSum;
			size_t					_kltIters;

			std::vector<KLTPType*>	_patches;
			std::vector<PoseType>	_poses;
			Time					_time;
			double					_fps;
			size_t					_iter;
			std::vector<Image>		_pyramid;

			void createPyramid();
			void initialize();
			void drawRect( Image& img );

	};

	inline void KLTWindow::onTimeout()
	{
		std::vector<size_t>	  trackedIndices;
		_kltTime.reset();	
		_klt.trackFeatures( trackedIndices, _poses, _patches, _pyramid[ 0 ] );
		//_klt.trackMultiscale( trackedIndices, _poses, _patches, _pyramid );

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

		//std::cout << "Pose: " << _poses.back().transformation() << std::endl;

		_window.update();
	}

	inline void KLTWindow::initialize()
	{
		// we take a patch in the middle
		KLTPType* p = new KLTPType();

		size_t stride;
		size_t phalf = p->size() >> 1;
		size_t x = _pyramid[ 0 ].width() / 2;
		size_t y = _pyramid[ 0 ].height() / 2;
		const uint8_t* ptr = _pyramid[ 0 ].map( &stride );
		p->position().x = x;
		p->position().y = y;

		// TODO: patch update should take the center pixel and a stride
		p->update( ptr + ( y - phalf ) * stride + x - phalf, stride );
		_pyramid[ 0 ].unmap( ptr );

		_patches.push_back( p );
		_poses.push_back( PoseType() );

		Matrix3f m;
		m.setIdentity();

		m.setAffine( Math::deg2Rad( 10.5f ),
					 Math::deg2Rad( 1.0f ),
					 1.01f, 1.02f,
					 x-phalf+5, y-phalf+5 );

		_poses.back().set( m );

	}
	
	inline void KLTWindow::drawRect( Image & img )
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

			_poses[ 0 ].transform( a, p0 );
			_poses[ 0 ].transform( b, p1 );
			_poses[ 0 ].transform( c, p2 );
			_poses[ 0 ].transform( d, p3 );

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

	inline void KLTWindow::createPyramid()
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

#endif
