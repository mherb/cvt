#ifndef CVT_KLT_WINDOW_H
#define CVT_KLT_WINDOW_H

#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>

#include <cvt/vision/FAST.h>
#include <cvt/io/RawVideoReader.h>
#include <cvt/io/Resources.h>
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
			//typedef GA2<float>			PoseType;
			//typedef Sim2<float>			PoseType;
			typedef Translation2D<float>	PoseType;
			typedef KLTPatch<16, PoseType>	KLTPType;

			KLTWindow( VideoInput & video ) :
				_window( "KLT" ),	
				_video( video ),
				_klt( 3 ),
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
				_window.update();				

				_pyramid.resize( 3 );
				_video.frame().convert( _pyramid[ 0 ], IFormat::GRAY_UINT8 );
			
				redetectFeatures( _pyramid[ 0 ] );
				std::cout << "Numinitial: " << _patches.size() << std::endl;

				_imView.setImage( _pyramid[ 0 ] );
				_fps = 0.0;
				_iter = 0;
			}

			~KLTWindow()
			{
				Application::unregisterTimer( _timerId );
			}

			void onTimeout();

		private:
			Image					_lena;
			Recti					_currR;
			int						_offX;
			int						_offY;
			uint32_t				_timerId;
			Window					_window;
			ImageView				_imView;
			VideoInput &			_video;

			KLTTracker<PoseType>		_klt;
			Time						_kltTime;
			double						_kltTimeSum;
			size_t						_kltIters;

			std::vector<KLTPType*>		_patches;
			std::vector<PoseType>		_poses;
			Time						_time;
			double						_fps;
			size_t						_iter;
			std::vector<Image>			_pyramid;

			void drawFeatures( Image & img, const std::vector<size_t> & indices );

			void redetectFeatures( const Image & img );

			void removeLost( const std::vector<size_t> & trackedIndices );
			
			void createPyramid();

	};

	inline void KLTWindow::onTimeout()
	{
		_video.frame().convert( _pyramid[ 0 ], IFormat::GRAY_UINT8 );
		createPyramid();
				
		std::vector<size_t>	  trackedIndices;
	
		_kltTime.reset();	
		//_klt.trackFeatures( trackedIndices, _poses, _patches, _pyramid[ 0 ] );
		_klt.trackMultiscale( trackedIndices, _poses, _patches, _pyramid );

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
		title.sprintf( "KLT: tracked=%d, all=%d, fps=%0.2f, avgklt=%0.2fms", trackedIndices.size(), _patches.size(), _fps, _kltTimeSum/_kltIters );
		_window.setTitle( title );

		removeLost( trackedIndices );

		if( _patches.size() < 50 )
			redetectFeatures( _pyramid[ 0 ] );

		Image debug;
		_pyramid[ 0 ].convert( debug, IFormat::RGBA_UINT8 );
		drawFeatures( debug, trackedIndices );
		_imView.setImage( debug );
		
		_video.nextFrame();
	}
	
	inline void KLTWindow::drawFeatures( Image & img, const std::vector<size_t> & indices )
	{
		{
			GFXEngineImage ge( img );
			GFX g( &ge );
			g.color() = Color::GREEN;

			Vector2f p0, p1;
			Eigen::Vector2f p;
			Eigen::Vector2f pp;
		
			size_t idx;
			for( size_t i = 0; i < indices.size(); i++ ){
				idx = indices[ i ];
				p0 = _patches[ idx ]->position();
				p[ 0 ] = p0[ 0 ]; p[ 1 ] = p0[ 1 ];
				
				_poses[ idx ].transformInverse( pp, p );
				EigenBridge::toCVT( p1, pp );

				g.drawLine( p0, p1 );
			}
		}
	}

	inline void KLTWindow::redetectFeatures( const Image & img )
	{
		std::vector<Vector2f> features;	
		VectorVector2Inserter<float> inserter( features );
		FAST::detect9( img, 30, inserter, 20 );


		for( size_t i = 0; i < _patches.size(); i++ ){
			const Vector2f & p = _patches[ i ]->position();

			std::vector<Vector2f>::iterator it = features.begin();
			while( it != features.end() ){
				if( ( p - *it ).lengthSqr() < 100.0f ){
					it = features.erase( it );
				} else {
					it++;
				}

			}
		}

		size_t oldSize = _patches.size();
		KLTPType::extractPatches( _patches, features, img );
		for( size_t i = oldSize; i < _patches.size(); i++ ){
			_poses.push_back( PoseType() );
		}
	}

	inline void KLTWindow::removeLost( const std::vector<size_t> & trackedIndices )
	{
		std::vector<size_t>::const_iterator idx = trackedIndices.begin();
		const std::vector<size_t>::const_iterator idxEnd = trackedIndices.end();

		size_t savePos = 0;
		while( idx != idxEnd ){
			if( savePos != *idx ){
				// savepos is lost, swap it:
				KLTPType* lost = _patches[ savePos ];
				_patches[ savePos ] = _patches[ *idx ];
				_patches[ *idx ] = lost;

				_poses[ savePos ] = _poses[ *idx ];
			}
			savePos++;
			idx++;
		}

		for( size_t i = savePos; i < _patches.size(); i++ ){
			delete _patches[ i ];
		}
		_patches.erase( _patches.begin() + trackedIndices.size(), _patches.end() );
		_poses.erase( _poses.begin() + trackedIndices.size(), _poses.end() );		
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
