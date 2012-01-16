#include "SLAMView.h"
#include <cvt/util/EigenBridge.h>

namespace cvt
{
	SLAMView::SLAMView() : 
		_trans( -10 ),
		_near( 0.1f ),
		_far( 100.0f ),
		_numKeyframes( 0 ),
		_maxKeyframes( 200 ),
		_numPoints( 0 ),
		_maxPoints( 0 )
	{
		createGrid( 10 );
		createAxes();
		createKeyframeBuffer();

		_rot.setIdentity();
		_cam.setIdentity();

		// alloc maxpoints space
		resizePointBuffer( 5000 );
	}

	void SLAMView::setCamPose( const Matrix4f & m )
	{
		_cam = m.inverse();
	}

	void SLAMView::paintGLEvent( PaintEvent* )
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
		_axes.draw( GL_LINES, 0, 6 );

		// draw the keyframes		
		_keyframes.setColor( Color::YELLOW );
		_keyframes.draw( GL_LINES, 0, 6 * _numKeyframes );

		proj = persp * view * _cam; 
		_basicProg.setProjection( proj );

	
		// draw the current camera pose	
		_axes.draw( GL_LINES, 0, 6 );

		_basicProg.unbind();
	}

	void SLAMView::mousePressEvent( MousePressEvent* e )
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

	void SLAMView::mouseReleaseEvent( MouseReleaseEvent* e )
	{
		if( e->button() == 4 ) {
			_trans += 0.25f;
			update();
		} else if( e->button() == 5 ) {
			_trans -= 0.25f;
			update();
		}
	}

	void SLAMView::mouseMoveEvent( MouseMoveEvent* e )
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

	void SLAMView::resizeEvent( ResizeEvent* e )
	{
		_arcball.setViewportSize( e->width(), e->height() );
	}

	void SLAMView::createGrid( ssize_t halfRes )
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

	void SLAMView::createAxes()
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

	void SLAMView::createKeyframeBuffer()
	{
		_keyframesAxesBuffer.alloc( GL_DYNAMIC_DRAW, sizeof( GL_FLOAT ) * 3 * 6 * _maxKeyframes, NULL );
		_keyframes.setVertexData( _keyframesAxesBuffer, 3, GL_FLOAT );
	}

	void SLAMView::mapChanged( const SlamMap & map )
	{
		// update the keyframe poses
		// update the features
		size_t numFeat = map.numFeatures();
		resizePointBuffer( numFeat );

		GLfloat* data = (GLfloat*)_pointBuf.map();
		for( size_t i = 0; i < numFeat; i++ ){
			const MapFeature & f = map.featureForId( i );
			data[ 0 ] = ( float )f.estimate()[ 0 ];
			data[ 1 ] = ( float )f.estimate()[ 1 ];
			data[ 2 ] = ( float )f.estimate()[ 2 ];
			data += 3;
		}
		_pointBuf.unmap();
		_numPoints = numFeat;
		_points.setColor( Color::GREEN );

		// update the keyframes: 
		_numKeyframes = map.numKeyframes();
		data = ( GLfloat* )_keyframesAxesBuffer.map();
		for( size_t i = 0; i < _numKeyframes; i++ ){
			const Keyframe & kf = map.keyframeForId( i );
			updateSingleKeyframe( data, kf );
			data += 18; // 3 axis a 2 points a 3 coords
		}
		_keyframesAxesBuffer.unmap();
	}

	void SLAMView::resetCameraView()
	{
		_rot = _cam;
		_rot.setTranslation( 0.0f, 0.0f, 0.0f );
		_trans = _cam[ 2 ][ 3 ] - 1.0f;
	}

	void SLAMView::updateSingleKeyframe( GLfloat* ptr, const Keyframe & kf )
	{
		Vector3f p;
		Vector3f pp;
		Matrix3f c2wR;
		Vector3f c2wt;

		{
			const Eigen::Matrix4d & pose = kf.pose().transformation();
			const Eigen::Matrix3d & R = pose.block<3, 3>( 0, 0 );
			const Eigen::Vector3d & t = pose.block<3, 1>( 0, 3 );
			EigenBridge::toCVT( c2wR, R );
			EigenBridge::toCVT( c2wt, t );
		}

		c2wR.transposeSelf();		
		c2wt = -c2wR * c2wt;

		p.x = 0.1f; p.y = 0.0f; p.z = 0.0f;
		pp = c2wR * p + c2wt;
		ptr[ 0 ] = c2wt.x; ptr[ 1 ] = c2wt.y; ptr[ 2 ] = c2wt.z; ptr += 3;
		ptr[ 0 ] = pp.x;   ptr[ 1 ] = pp.y;   ptr[ 2 ] = pp.z;   ptr += 3;

		p.x = 0.0f; p.y = 0.1f; p.z = 0.0f;
		pp = c2wR * p + c2wt;
		ptr[ 0 ] = c2wt.x; ptr[ 1 ] = c2wt.y; ptr[ 2 ] = c2wt.z; ptr += 3;
		ptr[ 0 ] = pp.x;   ptr[ 1 ] = pp.y;   ptr[ 2 ] = pp.z;   ptr += 3;

		p.x = 0.0f; p.y = 0.0f; p.z = 0.1f;
		pp = c2wR * p + c2wt;
		ptr[ 0 ] = c2wt.x; ptr[ 1 ] = c2wt.y; ptr[ 2 ] = c2wt.z; ptr += 3;
		ptr[ 0 ] = pp.x;   ptr[ 1 ] = pp.y;   ptr[ 2 ] = pp.z; 
	}

	void SLAMView::resizePointBuffer( size_t newSize ) 
	{
		if( newSize > _maxPoints ){
			newSize += 500;
			_pointBuf.alloc( GL_DYNAMIC_DRAW, sizeof( GLfloat ) * 3 * newSize, NULL );
			_maxPoints = newSize;
		}
		_points.setVertexData( _pointBuf, 3, GL_FLOAT );
	}

	void SLAMView::clearMap()
	{
		_cam.setIdentity();
		_numPoints = 0;
	}
}
