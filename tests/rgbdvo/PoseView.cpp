#include <PoseView.h>
#include <cvt/util/EigenBridge.h>

namespace cvt
{
    PoseView::PoseView() :
        _trans( 0.0f, 0.0f, -10.0f ),
        _near( 0.1f ),
        _far( 100.0f ),
        _arcball( 320, 240, 2.0f ),
        _numKeyframes( 0 ),
        _maxKeyframes( 5000 )
    {
        createGrid( 20 );
        createAxes();
        createKeyframeBuffer();

        _offset.setIdentity();
        _rot.setIdentity();
        _cam.setIdentity();
        _gtPose.setIdentity();
    }

    void PoseView::setCamPose( const Matrix4f & m )
    {
        _cam = _offset * m;
        update();
    }

    void PoseView::setGTPose( const Matrix4f & m )
    {
        _gtPose = _offset * m;
        update();
    }

    void PoseView::setOffsetPose( const Matrix4f& pose )
    {
        _offset = pose.inverse();
    }

    void PoseView::paintGLEvent( PaintEvent* )
    {
        Recti r = rect();
        float asp = ( float )r.width / ( float )r.height;
        setViewport( 0, 0, r.width, r.height );

        Matrix4f persp;
        GL::perspective( persp, 60.0f, asp, _near, _far );

        Matrix4f view, proj;
        view.setIdentity();
        view[ 0 ][ 3 ] = _trans[ 0 ];
        view[ 1 ][ 3 ] = _trans[ 1 ];
        view[ 2 ][ 3 ] = _trans[ 2 ];
        view *= _rot;

        proj = persp * view;

        _basicProg.bind();

        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT );

        _basicProg.setProjection( proj );

        // draw the grid
        glLineWidth( 1.0f );
        _grid.draw( GL_LINES, 0, _numLines );

        // draw the keyframes
        _keyframes.setColor( Color::RED );
        _keyframes.draw( GL_LINES, 0, 6 * _numKeyframes );

        _scenePoints.draw();

        // draw the current camera pose
        proj = persp * view * _cam;
        _basicProg.setProjection( proj );
        _axes.draw( GL_LINES, 0, 6 );

        // draw the ground truth axes
        proj = persp * view * _gtPose;
        _basicProg.setProjection( proj );
        _axes.setColor( Color::GREEN );
        _axes.draw( GL_LINES, 0, 6 );

        _basicProg.unbind();
    }

    void PoseView::mousePressEvent( MousePressEvent* e )
    {
        switch( e->button() ){
            case 1:
                _press.x = e->x;
                _press.y = e->y;
                break;
            case 2:
            case 3:
                _panPress.x = e->x;
                _panPress.y = e->y;
            default:
                break;
        }
    }

    void PoseView::mouseReleaseEvent( MouseReleaseEvent* e )
    {
        switch( e->button() ) {
            case 4:
                _trans.z += 0.25f;
                update();
                break;
            case 5:
                _trans.z -= 0.25f;
                update();
                break;
            default:
                break;
        }
    }

    void PoseView::mouseMoveEvent( MouseMoveEvent* e )
    {
        if( e->buttonMask() & 1 ) {
            Matrix4f rot;
            _arcball.getRotation( rot, _press.x, _press.y, e->x, e->y );

            _rot = rot * _rot;

            update();
            _press.x = e->x;
            _press.y = e->y;
        }

        if( e->buttonMask() & 2 || e->buttonMask() & 4 ) {
            _trans.x += 0.01f * ( e->x - _panPress.x );
            _trans.y -= 0.01f * ( e->y - _panPress.y );
            update();
            _panPress.x = e->x;
            _panPress.y = e->y;
        }
    }

    void PoseView::resizeEvent( ResizeEvent* e )
    {
        _arcball.setViewportSize( e->width(), e->height() );
    }

    void PoseView::createGrid( ssize_t halfRes )
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

    void PoseView::createAxes()
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

    void PoseView::createKeyframeBuffer()
    {
        _keyframesAxesBuffer.alloc( GL_DYNAMIC_DRAW, sizeof( GL_FLOAT ) * 3 * 6 * _maxKeyframes, NULL );
        _keyframes.setVertexData( _keyframesAxesBuffer, 3, GL_FLOAT );
    }

    void PoseView::resetCameraView()
    {
        Matrix4f R;
        R.setRotationXYZ( 0, Math::PI, Math::PI );
        _rot = R * _cam;
        _rot.setTranslation( 0.0f, 0.0f, 0.0f );
        _trans.z = _cam[ 2 ][ 3 ] - 1.0f;
    }

    void PoseView::setScenePoints( const ScenePoints& pts )
    {
        _scenePoints.setScenePoints( pts );
    }

    void PoseView::addKeyframe( const Matrix4f& world2Cam0 )
    {
        if( _numKeyframes == _maxKeyframes ){
            // need to reallocate!
            throw CVTException( "IMPLEMENT KEYFRAME REALLOCATION" );
        }

        Matrix4f world2Cam = _offset * world2Cam0;

        GLfloat* ptr = ( GLfloat* )_keyframesAxesBuffer.map();
        ptr += _numKeyframes * 18;

        Vector3f p, pp, o;
        p.x = 0.0f; p.y = 0.0f; p.z = 0.0f;
        o = world2Cam * p;

        p.x = 0.2f;
        p.y = 0.0f;
        p.z = 0.0f;
        pp = world2Cam * p;
        ptr[ 0 ] = o.x;
        ptr[ 1 ] = o.y;
        ptr[ 2 ] = o.z;
        ptr += 3;
        ptr[ 0 ] = pp.x;
        ptr[ 1 ] = pp.y;
        ptr[ 2 ] = pp.z;
        ptr += 3;

        p.x = 0.0f;
        p.y = 0.2f;
        p.z = 0.0f;
        pp = world2Cam * p;
        ptr[ 0 ] = o.x;
        ptr[ 1 ] = o.y;
        ptr[ 2 ] = o.z;
        ptr += 3;
        ptr[ 0 ] = pp.x;
        ptr[ 1 ] = pp.y;
        ptr[ 2 ] = pp.z;
        ptr += 3;

        p.x = 0.0f;
        p.y = 0.0f;
        p.z = 0.2f;
        pp = world2Cam * p;
        ptr[ 0 ] = o.x;
        ptr[ 1 ] = o.y;
        ptr[ 2 ] = o.z;
        ptr += 3;
        ptr[ 0 ] = pp.x;
        ptr[ 1 ] = pp.y;
        ptr[ 2 ] = pp.z;

        _keyframesAxesBuffer.unmap();

        _numKeyframes++;
    }
}
