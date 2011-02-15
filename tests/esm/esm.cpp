#include <iostream>
#include <cvt/gfx/Image.h>
#include <cvt/io/Camera.h>
#include <cvt/util/Time.h>
#include <cvt/math/Math.h>
#include <cvt/math/GaussNewton.h>
#include <cvt/math/LevenbergMarquard.h>
#include <cvt/util/Exception.h>
#include <cvt/vision/ESM.h>

#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/BasicTimer.h>
#include <cvt/util/Delegate.h>

using namespace cvt;

class RectSelectImageView : public ImageView
{
public:
	RectSelectImageView() : ImageView()
	{
		pts[ 0 ][ 0 ] = 0;
		pts[ 0 ][ 1 ] = 0;
		pts[ 1 ][ 0 ] = 0;
		pts[ 1 ][ 1 ] = 0;
		pts[ 2 ][ 0 ] = 0;
		pts[ 2 ][ 1 ] = 0;
		pts[ 3 ][ 0 ] = 0;
		pts[ 3 ][ 1 ] = 0;
	}
	
	void mouseMoveEvent( MouseMoveEvent* e )
	{
		if( ! ( e->buttonMask() & 1 ) ) return;
		
		int w, h;
		this->size( w, h );
		float x = ( float )e->x / ( float )w;
		float y = ( float )e->y / ( float )h;
		updateSelection( x, y );			
	}
	
	void mousePressEvent( MousePressEvent* e )
	{
		if( e->button() != 1 ) return;		
		
		int w, h;
		this->size( w, h );
		
		_firstClick[ 0 ] = ( float )( e->x ) / ( float )w;
		_firstClick[ 1 ] = ( float )( e->y ) / ( float )h;
		
		updateSelection( _firstClick[ 0 ] , _firstClick[ 1 ] );
		selectionDidStart.notify();
	}
	
	void mouseReleaseEvent( MouseReleaseEvent* e )
	{
		if( e->button() != 1 ) return;
		
		int w, h;
		this->size( w, h );
		float x = ( float )e->x / ( float )w;
		float y = ( float )e->y / ( float )h;

		updateSelection( x, y );
		
		selectionComplete.notify();
	}
	
	void paintEvent( PaintEvent* e , GFX* gfx ) 
	{
		ImageView::paintEvent( e, gfx );
		
		int w, h;
		
		this->size( w, h );
		gfx->color().set( 0.0f, 1.0f, 0.0f, 1.0f );		
		gfx->setLineWidth( 5 );
		gfx->drawLine( w*pts[ 0 ][ 0 ], h*pts[ 0 ][ 1 ], w*pts[ 1 ][ 0 ], h*pts[ 1 ][ 1 ] );
		gfx->drawLine( w*pts[ 1 ][ 0 ], h*pts[ 1 ][ 1 ], w*pts[ 2 ][ 0 ], h*pts[ 2 ][ 1 ] );
		gfx->drawLine( w*pts[ 2 ][ 0 ], h*pts[ 2 ][ 1 ], w*pts[ 3 ][ 0 ], h*pts[ 3 ][ 1 ] );
		gfx->drawLine( w*pts[ 3 ][ 0 ], h*pts[ 3 ][ 1 ], w*pts[ 0 ][ 0 ], h*pts[ 0 ][ 1 ] );
		update();
	}
	
	void selectedRect( Rectf & r )
	{
		r.x = Math::min( pts[ 0 ][ 0 ], pts[ 2 ][ 0 ] );
		r.y = Math::min( pts[ 0 ][ 1 ], pts[ 2 ][ 1 ] );
		r.width = Math::abs( pts[ 1 ][ 0 ] - pts[ 0 ][ 0 ] );
		r.height = Math::abs( pts[ 1 ][ 1 ] - pts[ 2 ][ 1 ] );
	}
	
	void updatePoints( Eigen::Matrix<double, 3, 4> & pt, float width, float height )
	{
		for( size_t i = 0; i < 4; i++ ){
			pts[ i ][ 0 ] = ( float ) ( pt( 0, i ) / pt( 2, i ) ) / width;
			pts[ i ][ 1 ] = ( float ) ( pt( 1, i ) / pt( 2, i ) ) / height;
		}
	}
	
	// normalized dimensions!
	Vector2f pts[ 4 ];
	Signal<void>	selectionComplete;
	Signal<void>	selectionDidStart;

private:
	Vector2f _firstClick;

	void updateSelection( float x, float y )
	{
		pts[ 0 ][ 0 ] = _firstClick[ 0 ];
		pts[ 0 ][ 1 ] = _firstClick[ 1 ];
		pts[ 1 ][ 0 ] = x;
		pts[ 1 ][ 1 ] = _firstClick[ 1 ];
		pts[ 2 ][ 0 ] = x;
		pts[ 2 ][ 1 ] = y;
		pts[ 3 ][ 0 ] = _firstClick[ 0 ];
		pts[ 3 ][ 1 ] = y;
	}
};

class EsmWindow : public Window
{
	public:
		EsmWindow( Camera * cam ) :
			Window( "ESM"),
			_bt( 10 ) /* update in ms */,
			_cam( cam ),
			_templateView(),
			_tempMov( &_templateView ),
			_camView(),
			_camMov( &_camView ),
			_camImage( cam->width(), cam->height(), IFormat::BGRA_UINT8 ),
			_termCrit( TERM_MAX_ITER | TERM_COSTS_THRESH ),
			_costFunc( 5 ),/* huber */ 
			_selectionReady( false )
		{
			this->setVisible( true );
			this->setSize( 800, 600 );

			// container for the template image
			_tempMov.setSize( 160, 120 );
			_tempMov.setTitle( "Template Image" );
			this->addWidget( &_tempMov );

			// container for the template image
			_camMov.setSize( 320, 240 );
			_camMov.setTitle( "Camera Image" );
			this->addWidget( &_camMov );

			_cam->nextFrame();
			_camView.setImage( _cam->frame() );
			
			// timer to call the loop:
			_runLoopDelegate = new  Delegate<void ( BasicTimer* )>( this, &EsmWindow::loop );
			_bt.timeout.add( _runLoopDelegate );
			_bt.start();
			
			_selectionDelegate = new Delegate< void () >( this, &EsmWindow::newTemplateSelection );
			_camView.selectionComplete.add( _selectionDelegate );
			
			_selectionDidStart = new Delegate< void () >( this, &EsmWindow::selectionStart );
			_camView.selectionDidStart.add( _selectionDidStart );
			
			
			_imgFloatGray.reallocate( _cam->width(), _cam->height(), IFormat::GRAY_FLOAT );			
			_time.reset();
			_iters = 0;
			
			_termCrit.setCostThreshold( 0.00001 );
			_termCrit.setMaxIterations( 40 );
		}
		
		~EsmWindow()
		{
			_bt.timeout.remove( _runLoopDelegate );
			delete _runLoopDelegate;
			
			_camView.selectionComplete.remove( _selectionDelegate );
			delete _selectionDelegate;			
		}
	
		void loop( BasicTimer* )
		{
			_cam->nextFrame();
			_cam->frame().convert( _camImage );
			_camView.setImage( _camImage );
			_camImage.convert( _imgFloatGray );

			if( _selectionReady ){
				_esm.updateInput( &_imgFloatGray );
				_gn.optimize( _esm, _costFunc, _termCrit );
								
				Eigen::Matrix<double, 3, 4> pt = _esm.pose().transformation() * _points;
				_camView.updatePoints( pt, _cam->width(), _cam->height() );
			}
			_iters++;
			
			if( _time.elapsedSeconds() > 2 ){
				char buf[ 100 ];
				sprintf( buf, "Camera Image fps: %0.1f", _iters / _time.elapsedSeconds() );
				_camMov.setTitle( buf );
				_time.reset();
				_iters = 0;
			}
		}

		void newTemplateSelection()
		{
			int w, h;
			_camView.size( w, h );

			float sx = ( float )_cam->width();
			float sy = ( float )_cam->height();

			Rectf rect;
			_camView.selectedRect( rect );
			std::cout << sx << std::endl;
			std::cout << sy << std::endl;
			rect.x *= sx;
			rect.y *= sy;
			rect.width *= sx;
			rect.height *= sy;

			std::cout << rect << std::endl;

			Recti roi;
			roi.x = ( int )rect.x;
			roi.y = ( int )rect.y;
			roi.width = Math::max( ( int )rect.width, 5 );
			roi.height = Math::max( ( int )rect.height, 5);

			Image tmp( _imgFloatGray, &roi );
			_esm.updateTemplate( tmp );

			Image tmpu8( _camImage, &roi );
			_templateView.setImage( tmpu8 );

			_points( 0, 0 ) = 0;
			_points( 1, 0 ) = 0;
			_points( 2, 0 ) = 1;

			_points( 0 , 1 ) = roi.width;
			_points( 1 , 1 ) = 0;
			_points( 2 , 1 ) = 1;
			
			_points( 0 , 2 ) = roi.width;
			_points( 1 , 2 ) = roi.height; 
			_points( 2 , 2 ) = 1;
			
			_points( 0 , 3 ) = 0;
			_points( 1 , 3 ) = roi.height; 
			_points( 2 , 3 ) = 1;

			// set the homography
			_esm.updateInput( &_imgFloatGray );
			_esm.setPose( 0, 0, 1, 1, roi.x, roi.y, 0, 0 );
			
			_selectionReady = true;
		}
	
		void selectionStart()
		{
			_selectionReady = false;
		}
			
	private:
		BasicTimer				_bt;
		Camera*					_cam;
		ImageView				_templateView;
		Moveable				_tempMov;		
		RectSelectImageView		_camView;
		Moveable				_camMov;
	
		Image							_imgFloatGray;
		Image							_camImage;	
		Eigen::Matrix<double, 3, 4>		_points;	
	
		Delegate<void ( BasicTimer* )>*	 _runLoopDelegate;
		Delegate<void ()>*				 _selectionDelegate;
		Delegate<void ()>*				 _selectionDidStart;
	
		ESM<double>					_esm;
		TerminationCriteria<double>	_termCrit;
		//GaussNewton<double>			_gn;
		LevenbergMarquard<double>			_gn;
	
		// costfunction to use
		//SquaredDistance<double, double> _costFunc;
		RobustHuber<double, double> _costFunc;

		bool			_selectionReady;
		Time			_time;
		Time			_esmTime;
		size_t			_iters;
};

int main( void )
{
	Camera::updateInfo();
	size_t numCams = Camera::count();
	
	std::cout << "Overall number of Cameras: " << numCams << std::endl;
	if( numCams == 0 ){
		std::cout << "Please connect a camera!" << std::endl;
		return 0;
	}

	for( size_t i = 0; i < numCams; i++ ){
		const CameraInfo & info = Camera::info( i );
		std::cout << "Camera " << i << ": " << info << std::endl;
	}

	size_t selection = numCams;
	std::cout << "Select camera: ";
	std::cin >> selection;
	while ( selection >= numCams ){
		std::cout << "Index out of bounds -> select camera in Range:";
		std::cin >> selection;
	}

	try {
	Camera * cam;
	cam = Camera::get( selection, 640, 480, 60, IFormat::UYVY_UINT8 );
	cam->startCapture();
	EsmWindow window( cam );

	Application::run();
	cam->stopCapture();
	delete cam;
	} catch( const Exception & e ){
		std::cout << e.what() << std::endl;
	}

	return 0;
}
