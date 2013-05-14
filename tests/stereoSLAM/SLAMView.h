#ifndef CVT_SLAMVIEW_H
#define CVT_SLAMVIEW_H

#include <cvt/geom/ArcBall.h>
#include <cvt/gui/GLView.h>
#include <cvt/gl/progs/GLBasicProg.h>

#include <cvt/vision/slam/SlamMap.h>

namespace cvt
{
	class SLAMView : public GLView
	{
		public:
			SLAMView();
			
			void setCamPose( const Matrix4f & m ); 
			void resetCameraView();
			void mapChanged( const SlamMap & map );
			void clearMap();

		protected:

			void paintGLEvent( PaintEvent& );
			void mousePressEvent( MousePressEvent& e );
			void mouseReleaseEvent( MouseReleaseEvent& e );
			void mouseMoveEvent( MouseMoveEvent& e );
			void resizeEvent( ResizeEvent& e );

		private:
			// view transform (virtual cam)
			Matrix4f	_rot;
			Vector3f	_trans;
			float		_near;
			float		_far;

			// Current Camera pose
			Matrix4f	_cam;

			ArcBall		_arcball;
			Vector2i	_press;
			Vector2i	_panPress;

			GLBasicProg	_basicProg;

			// base level grid
			GLVertexArray	_grid;
			GLBuffer		_gridLines;
			size_t			_numLines;

			GLVertexArray	_axes;
			GLBuffer		_axesBuf;
			GLBuffer		_axesColBuf;

			size_t			_numKeyframes;
			size_t			_maxKeyframes;
			GLVertexArray	_keyframes;
			GLBuffer		_keyframesAxesBuffer;

			GLVertexArray	_points;
			GLBuffer		_pointBuf;
			size_t			_numPoints;
			size_t			_maxPoints;

			void createGrid( ssize_t halfRes );
			void createAxes();
			void createKeyframeBuffer();
			void updateSingleKeyframe( GLfloat* ptr, const Keyframe & kf );
			void resizePointBuffer( size_t newSize );
	};
}

#endif
