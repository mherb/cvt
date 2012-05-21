#include <iostream>

#include <cvt/io/Resources.h>
#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/Vision.h>
#include <cvt/util/Signal.h>

#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/gfx/ifilter/ITransform.h>

#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/geom/Line2D.h>

using namespace cvt;

class ClickableImageView : public ImageView
{
	public:
		ClickableImageView() : ImageView()
		{
		}

		Signal<Vector2f*>	pointSelected;

	private:
		Vector2f point;
		void mousePressEvent( MousePressEvent* e )
		{
			// emit signal with point
			int x, y;
			e->position( x, y );
			int w, h;
			size( w, h );
			point.x = (float)x / ( float )w;
			point.y = (float)y / ( float )h;
			pointSelected.notify( &point );
		}
};

class MyWindow : public Window
{
	public:
		MyWindow( const Matrix3f & fundamental,
			      const Image& i0, const Image& i1 ) : 
			Window( "Stereo" ),
			_fundamental( fundamental ),
			_image0( i0 ),
			_image1( i1 ),
			_mov0( &_view0 ),
			_mov1( &_view1 )
		{
			initGui();
			_view0.setImage( _image0 );
			_view1.setImage( _image1 );

			Delegate<void (Vector2f*)> d( this, &MyWindow::updateImages );
			_view0.pointSelected.add( d );
		}

		void updateImages( Vector2f* p )
		{
			Vector3f point( p->x * _image0.width(), p->y * _image0.height(), 1.0f );
			Vector3f pp = _fundamental * point;

			Image tmp;
			_image0.convert( tmp, IFormat::RGBA_UINT8 );

			{
				GFXEngineImage ge( tmp );
				GFX g( &ge );
				g.color() = Color::GREEN;

				g.fillRect( (int)point.x-5, (int)point.y-5, 11, 11 );
			}
			_view0.setImage( tmp );

			// the line
			_image1.convert( tmp, IFormat::RGBA_UINT8 );
			Line2Df line( pp );
			{
				GFXEngineImage ge( tmp );
				GFX g( &ge );
				g.color() = Color::GREEN;
				g.setLineWidth( 3 );

				Vector2f p0, p1;
				pointsForLine( p0, p1, line );
				g.drawLine( p0, p1 );
			}
			_view1.setImage( tmp );
		}

	private:
		Matrix3f	_fundamental;
		Image		_image0;
		Image		_image1;

		ClickableImageView	_view0;
	    ImageView			_view1;
		Moveable			_mov0, _mov1;

		void initGui()
		{
			_mov0.setSize( 320, 240 );
			_mov1.setSize( 320, 240 );

			this->addWidget( &_mov0 );
			this->addWidget( &_mov1 );
		}

		void pointsForLine( Vector2f& p0, Vector2f& p1, const Line2Df& l )
		{
			p0.x = 0;
			p0.y = yComponentOfLine( p0.x, l ); 
			p1.x = _image1.width();
			p1.y = yComponentOfLine( p1.x, l ); 
		}

		float yComponentOfLine( float x, const Line2Df& l )
		{
			return ( -l[ 2 ] - l[ 0 ] * x ) / l[ 1 ];
		}
};

int main( int argc, char* argv[] )
{
	CameraCalibration camCalib0, camCalib1;
	camCalib0.load( argv[1] );
	camCalib1.load( argv[2] );


	Image i0( argv[3] );
	Image i1( argv[4] );
	Image id0, id1, tmp0, tmp1;
	
	Image warp;
	Vector3f radial = camCalib0.radialDistortion();
	Vector2f tangential = camCalib0.tangentialDistortion();
	float fx = camCalib0.intrinsics()[ 0 ][ 0 ];
	float fy = camCalib0.intrinsics()[ 1 ][ 1 ];
	float cx = camCalib0.intrinsics()[ 0 ][ 2 ];
	float cy = camCalib0.intrinsics()[ 1 ][ 2 ];

	id0.reallocate( i0.width() , i0.height() );
	warp.reallocate( i0.width(), i0.height() , IFormat::GRAYALPHA_FLOAT );
	IWarp::warpUndistort( warp, radial[ 0 ], radial[ 1 ], cx, cy, fx, fy, i0.width(), i0.height(), radial[ 2 ], tangential[ 0 ], tangential[ 1 ] );
	i0.convert( tmp0, IFormat::RGBA_UINT8 );
	IWarp::apply( id0, tmp0, warp );

	radial = camCalib1.radialDistortion();
	tangential = camCalib1.tangentialDistortion();
	fx = camCalib1.intrinsics()[ 0 ][ 0 ];
	fy = camCalib1.intrinsics()[ 1 ][ 1 ];
	cx = camCalib1.intrinsics()[ 0 ][ 2 ];
	cy = camCalib1.intrinsics()[ 1 ][ 2 ];
	IWarp::warpUndistort( warp, radial[ 0 ], radial[ 1 ], cx, cy, fx, fy, i1.width(), i1.height(), radial[ 2 ], tangential[ 0 ], tangential[ 1 ] );
	i1.convert( tmp0, IFormat::RGBA_UINT8 );
	IWarp::apply( id1, tmp0, warp );

	Matrix3f fundamental;
	Vision::composeFundamental( fundamental, 
							    camCalib0.intrinsics(),
								camCalib0.extrinsics(),
								camCalib1.intrinsics(),
								camCalib1.extrinsics() );

	MyWindow win( fundamental, id0, id1 );
	win.setSize( 800, 600 );
	win.setVisible( true );
	win.setMinimumSize( 320, 240 );

	Application::run();

	return 0;
}
