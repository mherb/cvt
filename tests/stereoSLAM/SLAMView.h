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
		SLAMView() : 
				_trans( -10 ),
				_near( 0.1f ),
				_far( 100.0f )
		{
			createGrid( 10 );
			createAxes();
			_rot.setIdentity();

			_ogl2World.setIdentity();
			_ogl2World.setRotationX( -Math::HALF_PI );

			_cam.setIdentity();
			_cam.setTranslation( 2.0f, 0.0f, 5.0f );
			_cam = _ogl2World * _cam;
		}
			
		void setCamPose( const Matrix4f & m )
		{
			_cam = _ogl2World * m;
		}

		protected:

			void paintGLEvent( PaintEvent* )
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

				proj = persp * view * _cam; 
				_basicProg.setProjection( proj );
				//_teapot.draw();
				_axes.draw( GL_LINES, 0, 6 );

				_basicProg.unbind();
			}


			void mousePressEvent( MousePressEvent* e )
			{
				if( e->button() == 1 ) {
					_press.x = e->x;
					_press.y = e->y;
				}
			}


			void mouseReleaseEvent( MouseReleaseEvent* e )
			{
				if( e->button() == 4 ) {
					_trans += 0.25f;
					update();
				} else if( e->button() == 5 ) {
					_trans -= 0.25f;
					update();
				}
			}

			void mouseMoveEvent( MouseMoveEvent* e )
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


			void resizeEvent( ResizeEvent* e )
			{
				_arcball.setViewportSize( e->width(), e->height() );
			}


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
			GLBuffer		_pointData;
			size_t			_numPoints;
			size_t			_maxPoints;

			void createGrid( size_t halfRes )
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

			void createAxes()
			{
				_axesBuf.alloc( GL_STATIC_DRAW, sizeof( GL_FLOAT ) * 3 * 6, NULL );
				GLfloat * data = ( GLfloat* )_axesBuf.map();
				data[ 0 ] = 0.0f; data[ 1 ] = 0.0f;	data[ 2 ] = 0.0f;
				data[ 3 ] = 1.0f; data[ 4 ] = 0.0f;	data[ 5 ] = 0.0f;
				data[ 6 ] = 0.0f; data[ 7 ]  = 0.0f; data[ 8 ]  = 0.0f;
				data[ 9 ] = 0.0f; data[ 10 ] = 1.0f; data[ 11 ] = 0.0f;
				data[ 12 ] = 0.0f; data[ 13 ] = 0.0f; data[ 14 ] = 0.0f;
				data[ 15 ] = 0.0f; data[ 16 ] = 0.0f; data[ 17 ] = 1.0f;
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

	};
}

#endif
