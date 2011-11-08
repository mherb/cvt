#ifndef CVT_SLAMVIEW_H
#define CVT_SLAMVIEW_H

#include <cvt/geom/ArcBall.h>
#include <cvt/gui/GLView.h>
#include <cvt/gl/progs/GLBasicProg.h>

namespace cvt
{
	class SLAMView : public GLView
	{
		public:
			SLAMView();
			
			void setCamPose( const Matrix4f & m ) { _cam = _ogl2World * m; }
			void resetCameraView();
			void addPoints( const std::vector<Vector4f> & points );

		protected:

			void paintGLEvent( PaintEvent* );
			void mousePressEvent( MousePressEvent* e );
			void mouseReleaseEvent( MouseReleaseEvent* e );
			void mouseMoveEvent( MouseMoveEvent* e );
			void resizeEvent( ResizeEvent* e );

		private:
			// view transform (virtual cam)
			Matrix4f	_rot;
			float		_trans;
			float		_near;
			float		_far;

			// Current Camera pose
			Matrix4f	_cam;
			Matrix4f	_ogl2World;

			ArcBall		_arcball;
			Vector2i	_press;

			GLBasicProg	_basicProg;

			// base level grid
			GLVertexArray	_grid;
			GLBuffer		_gridLines;
			size_t			_numLines;

			GLVertexArray	_axes;
			GLBuffer		_axesBuf;
			GLBuffer		_axesColBuf;

			GLVertexArray	_points;
			GLBuffer		_pointBuf;
			size_t			_numPoints;
			size_t			_maxPoints;

			void createGrid( size_t halfRes );
			void createAxes();
	};

	inline SLAMView::SLAMView() : 
				_trans( -10 ),
				_near( 0.1f ),
				_far( 100.0f ),
				_numPoints( 0 ),
				_maxPoints( 5000 )
	{
		createGrid( 10 );
		createAxes();

		_rot.setIdentity();

		_ogl2World.setIdentity();
		_ogl2World.setRotationX( Math::PI );

		_cam.setIdentity();
		_cam = _ogl2World * _cam;

		// alloc maxpoints space
		_pointBuf.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 3 * _maxPoints, NULL );
		_points.setVertexData( _pointBuf, 3, GL_FLOAT );
		_points.setColor( Color::RED );
	}

	inline void SLAMView::paintGLEvent( PaintEvent* )
	{
		Recti r = rect();
		float asp = ( float )r.width / ( float )r.height;
		setViewport( 0, 0, r.width, r.height );

		Matrix4f persp;	
		GL::perspective( persp, 60.0f, asp, _near, _far );

		Matrix4f view, proj;
		view.setIdentity();
		view[ 2 ][ 3 ] = _trans;
		view *= _rot;

		proj = persp * view;

		_basicProg.bind();

		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );

		_basicProg.setProjection( proj );

		glLineWidth( 1.0f );
		_grid.draw( GL_LINES, 0, _numLines );

		// draw the points
		if( _numPoints ){
			glPointSize( 4.0f );
			_points.setColor( Color::GREEN );
			_points.draw( GL_POINTS, 0, _numPoints );
		}
		
		// draw the origin
		//_axes.draw( GL_LINES, 0, 6 );

		proj = persp * view * _cam; 
		_basicProg.setProjection( proj );
		//_teapot.draw();
		_axes.draw( GL_LINES, 0, 6 );

		_basicProg.unbind();
	}

	inline void SLAMView::mousePressEvent( MousePressEvent* e )
	{
		switch( e->button() ){
			case 1:
				_press.x = e->x;
				_press.y = e->y;
				break;
			default:
				break;
		}
	}

	inline void SLAMView::mouseReleaseEvent( MouseReleaseEvent* e )
	{
		if( e->button() == 4 ) {
			_trans += 0.25f;
			update();
		} else if( e->button() == 5 ) {
			_trans -= 0.25f;
			update();
		}
	}

	inline void SLAMView::mouseMoveEvent( MouseMoveEvent* e )
	{
		if( e->buttonMask() & 1 ) {
			Matrix4f rot;
			_arcball.getRotation( rot, _press.x, _press.y, e->x, e->y );

			_rot = rot * _rot;

			update();
			_press.x = e->x;
			_press.y = e->y;
		}
	}

	inline void SLAMView::resizeEvent( ResizeEvent* e )
	{
		_arcball.setViewportSize( e->width(), e->height() );
	}

	inline void SLAMView::createGrid( size_t halfRes )
	{
		_gridLines.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 3 * 2 * 2 * ( 2 * halfRes + 1 ), NULL );

		_numLines = 2 * 2 * ( 2 * halfRes + 1 );

		GLfloat * lineData = ( GLfloat* )_gridLines.map();
		for( float x = -halfRes; x <= halfRes; x+=1.0f  ){
			lineData[ 0 ] = x;
			lineData[ 1 ] = 0.0f;
			lineData[ 2 ] = -halfRes;
			lineData[ 3 ] = x;
			lineData[ 4 ] = 0.0f;
			lineData[ 5 ] = halfRes;

			lineData[ 6  ] = -halfRes;
			lineData[ 7  ] = 0;
			lineData[ 8  ] = x;
			lineData[ 9  ] = halfRes;
			lineData[ 10 ] = 0;
			lineData[ 11 ] = x;
			lineData += 12;
		}

		_gridLines.unmap();

		_grid.setVertexData( _gridLines, 3, GL_FLOAT );
		_grid.setColor( Color::WHITE );
	}

	inline void SLAMView::createAxes()
	{
		_axesBuf.alloc( GL_STATIC_DRAW, sizeof( GL_FLOAT ) * 3 * 6, NULL );
		GLfloat * data = ( GLfloat* )_axesBuf.map();
		data[ 0 ] = 0.0f; data[ 1 ] = 0.0f;	data[ 2 ] = 0.0f; data += 3;
		data[ 0 ] = 0.2f; data[ 1 ] = 0.0f;	data[ 2 ] = 0.0f; data += 3;
		data[ 0 ] = 0.0f; data[ 1 ] = 0.0f;	data[ 2 ] = 0.0f; data += 3;
		data[ 0 ] = 0.0f; data[ 1 ] = 0.2f;	data[ 2 ] = 0.0f; data += 3;
		data[ 0 ] = 0.0f; data[ 1 ] = 0.0f;	data[ 2 ] = 0.0f; data += 3;
		data[ 0 ] = 0.0f; data[ 1 ] = 0.0f;	data[ 2 ] = 0.2f; 
		_axesBuf.unmap();
		_axes.setVertexData( _axesBuf, 3, GL_FLOAT );

		_axesColBuf.alloc( GL_STATIC_DRAW, sizeof( GL_FLOAT ) * 4 * 6, NULL );
		data = ( GLfloat* )_axesColBuf.map();
		data[ 0 ] = 1.0f; data[ 1 ] = 0.0f;	data[ 2 ] = 0.0f; data[ 3 ] = 1.0f;
		data += 4;
		data[ 0 ] = 1.0f; data[ 1 ] = 0.0f;	data[ 2 ] = 0.0f; data[ 3 ] = 1.0f;
		data += 4;

		data[ 0 ] = 0.0f; data[ 1 ] = 1.0f;	data[ 2 ] = 0.0f; data[ 3 ] = 1.0f;
		data += 4;
		data[ 0 ] = 0.0f; data[ 1 ] = 1.0f;	data[ 2 ] = 0.0f; data[ 3 ] = 1.0f;
		data += 4;

		data[ 0 ] = 0.0f; data[ 1 ] = 0.0f;	data[ 2 ] = 1.0f; data[ 3 ] = 1.0f;
		data += 4;
		data[ 0 ] = 0.0f; data[ 1 ] = 0.0f;	data[ 2 ] = 1.0f; data[ 3 ] = 1.0f;
		_axesColBuf.unmap();

		_axes.setVertexData( _axesBuf, 3, GL_FLOAT );
		_axes.setColorData( _axesColBuf, 4, GL_FLOAT );
	}

	inline void SLAMView::addPoints( const std::vector<Vector4f> & points )
	{
		size_t newSize = _numPoints + points.size();	

		if( newSize > _maxPoints ){
			// realloc and copy
		} else {
			GLfloat* data = (GLfloat*)_pointBuf.map();
			data += ( 3 * _numPoints );

			for( size_t i = 0; i < points.size(); i++ ){
				data[ 0 ] = points[ i ].x;
				data[ 1 ] = points[ i ].y;
				data[ 2 ] = points[ i ].z;
				data += 3;
			}

			_pointBuf.unmap();
		}
		_numPoints = newSize;
		_points.setColor( Color::GREEN );
	}

	inline void SLAMView::resetCameraView()
	{
		_rot = _cam;
		_rot.setTranslation( 0.0f, 0.0f, 0.0f );
		_trans = _cam[ 2 ][ 3 ] - 0.5f;
	}
}

#endif
