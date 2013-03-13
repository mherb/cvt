#ifndef CVT_POSEVIEW_H
#define CVT_POSEVIEW_H

#include <cvt/geom/ArcBall.h>
#include <cvt/gui/GLView.h>
#include <cvt/gl/progs/GLBasicProg.h>
#include <cvt/gl/GLLines.h>

namespace cvt
{
    class PoseView : public GLView
    {
        public:
            PoseView();

            /**
             *  \brief set the current estimated pose
             *  \param m    pose from World TO Camera
             */
            void setCamPose( const Matrix4f & m );

            /**
             *  \brief set the current ground truth pose
             *  \param m    pose from World TO Camera
             */
            void setGTPose( const Matrix4f & m );

            /**
             *  \brief will reset the virtual view slightly behind the current camera pose
             */
            void resetCameraView();

            /**
             *  \brief add another keyframe pose
             *  \param pose     pose from World TO Camera
             */
            void addKeyframe( const Matrix4f& pose );

            void setOffsetPose( const Matrix4f& pose );

        protected:
            void paintGLEvent( PaintEvent* );
            void mousePressEvent( MousePressEvent* e );
            void mouseReleaseEvent( MouseReleaseEvent* e );
            void mouseMoveEvent( MouseMoveEvent* e );
            void resizeEvent( ResizeEvent* e );

        private:
            // view transform (virtual cam)
            Matrix4f    _offset;
            Matrix4f	_rot;
            Vector3f	_trans;
            float		_near;
            float		_far;

            // Current Camera pose
            Matrix4f	_cam;

            // Current GT pose
            Matrix4f    _gtPose;

            ArcBall		_arcball;
            Vector2i	_press;
            Vector2i	_panPress;

            GLBasicProg	_basicProg;

            // base level grid
            GLVertexArray	_grid;
            GLBuffer        _gridLines;
            size_t          _numLines;

            GLVertexArray	_axes;
            GLBuffer        _axesBuf;
            GLBuffer        _axesColBuf;

            size_t          _numKeyframes;
            size_t          _maxKeyframes;
            GLVertexArray	_keyframes;
            GLBuffer        _keyframesAxesBuffer;

            void createGrid( ssize_t halfRes );
            void createAxes();
            void createKeyframeBuffer();
    };
}

#endif
