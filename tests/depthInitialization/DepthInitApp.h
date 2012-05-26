#ifndef CVT_DEPTHINIT_APP_H
#define CVT_DEPTHINIT_APP_H

#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/Application.h>
#include <cvt/vision/slam/stereo/DepthInitializer.h>
#include <cvt/vision/slam/stereo/ORBStereoInit.h>
#include <cvt/vision/slam/stereo/PatchStereoInit.h>
#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/util/Time.h>

#include <GLSceneView.h>
#include <cvt/gui/ParamSetView.h>

namespace cvt
{

    class DepthInitApp : public TimeoutHandler
    {
        public:
            DepthInitApp( std::vector<VideoInput*>& videos,
                          const std::vector<CameraCalibration>& calibrations );

            void onTimeout();

            void toggleStepping();

        private:
            std::vector<VideoInput*>&               _videos;
            const std::vector<CameraCalibration>&   _calibrations;
            DepthInitializer*                       _depthInit;
            ParamSetView                            _depthParamsView;
            Moveable                                _depthParamsMov;


            Window          _window;
            GLSceneView     _sceneView;
            ImageView       _undistortedView;
            uint32_t        _timerId;
            Button          _toggleStepping;
            bool            _stepping;

            Image           _undist0;
            Image           _undist1;

            // undistortion maps
            Image           _undistMap0;
            Image           _undistMap1;

            void initUndistortionMaps();

            void updatePointsView( const std::vector<DepthInitializer::DepthInitResult>& triangulated, const Image& color0 );
            void createStereoImage( Image& img, const Image& color0, const Image& color1 );
            void drawCorrespondences( Image& out, const std::vector<DepthInitializer::DepthInitResult>& triangulated, size_t offX );
    };

    inline DepthInitApp::DepthInitApp( std::vector<VideoInput*>& videos, const std::vector<CameraCalibration>& calibrations ) :
        _videos( videos ),
        _calibrations( calibrations ),
        //_depthInit( new ORBStereoInit( calibrations[ 0 ], calibrations[ 1 ] ) ),
        _depthInit( new PatchStereoInit( calibrations[ 0 ], calibrations[ 1 ], videos[ 0 ]->width(), videos[ 0 ]->height() ) ),
        _depthParamsView( _depthInit->parameters() ),
        _depthParamsMov( &_depthParamsView ),
        _window( "DepthView" ),
        _toggleStepping( "Toggle Stepping" ),
        _stepping( false )
    {
        _window.setSize( 800, 600 );
        WidgetLayout wl;
        wl.setAnchoredLeftRight( 3, 3 );
        wl.setRelativeTopBottom( 0.01f, 0.49f );
        _window.addWidget( &_sceneView, wl );

        wl.setRelativeTopBottom( 0.51f, 0.94f );
        _window.addWidget( &_undistortedView, wl );
        wl.setRelativeTopBottom( 0.95f, 0.99f );
        _window.addWidget( &_toggleStepping, wl );

        Delegate<void ()> t( this, &DepthInitApp::toggleStepping );
        _toggleStepping.clicked.add( t );

        _depthParamsMov.setSize( 200, 400 );
        _depthParamsMov.setTitle( "Depth Initializer Parameters" );
        _window.addWidget( &_depthParamsMov );

        _window.setVisible( true );

        initUndistortionMaps();

        _timerId = Application::registerTimer( 20, this );
    }

    void DepthInitApp::toggleStepping()
    {
        _stepping = !_stepping;
    }

    inline void DepthInitApp::onTimeout()
    {
        if( _stepping )
            return;

        Time time;
        Image color0, color1;
        Image gray0, gray1;

        for( size_t i = 0; i < _videos.size(); i++ ){
            _videos[ i ]->nextFrame();
        }

        // convert & undistort
        _videos[ 0 ]->frame().convert( color0, IFormat::RGBA_UINT8 );
        color0.convert( gray0, IFormat::GRAY_UINT8 );
        IWarp::apply( _undist0, gray0, _undistMap0 );


        _videos[ 1 ]->frame().convert( color1, IFormat::RGBA_UINT8 );
        color1.convert( gray1, IFormat::GRAY_UINT8 );
        IWarp::apply( _undist1, gray1, _undistMap1 );

        // triangulate with depthinitializer
        std::vector<DepthInitializer::DepthInitResult> initresult;
        std::vector<Vector2f> avoidPos;
        _depthInit->triangulateFeatures( initresult, avoidPos, _undist0, _undist1 );

        updatePointsView( initresult, color0 );

        Image stereo;
        createStereoImage( stereo, _undist0, _undist1 );
        drawCorrespondences( stereo, initresult, _undist0.width() );
        _undistortedView.setImage( stereo );

        std::cout << time.elapsedMilliSeconds() << "ms - triangulated: " << initresult.size() << std::endl;
    }

    inline void DepthInitApp::initUndistortionMaps()
    {
        size_t w0 = _videos[ 0 ]->width();
        size_t h0 = _videos[ 0 ]->height();
        size_t w1 = _videos[ 1 ]->width();
        size_t h1 = _videos[ 1 ]->height();

        _undistMap0.reallocate( w0, h0, IFormat::GRAYALPHA_FLOAT );
        _undistMap1.reallocate( w1, h1, IFormat::GRAYALPHA_FLOAT );

        const CameraCalibration& c0 = _calibrations[ 0 ];
        const CameraCalibration& c1 = _calibrations[ 1 ];
        Vector3f radial = c0.radialDistortion();
        Vector2f tangential = c0.tangentialDistortion();
        float fx = c0.intrinsics()[ 0 ][ 0 ];
        float fy = c0.intrinsics()[ 1 ][ 1 ];
        float cx = c0.intrinsics()[ 0 ][ 2 ];
        float cy = c0.intrinsics()[ 1 ][ 2 ];
        IWarp::warpUndistort( _undistMap0, radial[ 0 ], radial[ 1 ], cx, cy, fx, fy, w0, h0, radial[ 2 ], tangential[ 0 ], tangential[ 1 ] );

        radial = c1.radialDistortion();
        tangential = c1.tangentialDistortion();
        fx = c1.intrinsics()[ 0 ][ 0 ];
        fy = c1.intrinsics()[ 1 ][ 1 ];
        cx = c1.intrinsics()[ 0 ][ 2 ];
        cy = c1.intrinsics()[ 1 ][ 2 ];
        IWarp::warpUndistort( _undistMap1, radial[ 0 ], radial[ 1 ], cx, cy, fx, fy, w1, h1, radial[ 2 ], tangential[ 0 ], tangential[ 1 ] );

    }

    inline void DepthInitApp::updatePointsView( const std::vector<DepthInitializer::DepthInitResult>& triangulated,
                                                const Image& color0 )
    {
        IMapScoped<const uint8_t> map( color0 );

        std::vector<Vector3f> vertices;
        std::vector<Vector4f> colors;

        vertices.resize( triangulated.size() );
        colors.resize( triangulated.size() );

        for( size_t i = 0; i < triangulated.size(); i++ ){
            const DepthInitializer::DepthInitResult& t = triangulated[ i ];
            Vector3f& p3d = vertices[ i ];
            Vector4f& c = colors[ i ];

            p3d.x = t.point3d.x / t.point3d.w;
            p3d.y = t.point3d.y / t.point3d.w;
            p3d.z = t.point3d.z / t.point3d.w;

            const uint8_t* cptr = map.ptr() + (int)t.meas0.y * map.stride() + (int)t.meas0.x * 4;
            c.x = cptr[ 0 ] / 255.0f;
            c.y = cptr[ 1 ] / 255.0f;
            c.z = cptr[ 2 ] / 255.0f;
            c.w = 1.0f;
        }

        ScenePoints pts( "triangulated" );
        pts.setVerticesWithColor( &vertices[ 0 ], &colors[ 0 ], vertices.size() );

        _sceneView.setScenePoints( pts );
    }

    inline void DepthInitApp::createStereoImage( Image& img, const Image& color0, const Image& color1 )
    {
        Image c0, c1;
        color0.convert( c0, IFormat::RGBA_UINT8 );
        color1.convert( c1, IFormat::RGBA_UINT8 );

        img.reallocate( c0.width() + c1.width(),
                        Math::max( c0.height(), c1.height() ),
                        c0.format() );

        img.copyRect( 0, 0, c0, c0.rect() );
        img.copyRect( c0.width(), 0, c1, c1.rect() );
    }

    inline void DepthInitApp::drawCorrespondences( Image& out, const std::vector<DepthInitializer::DepthInitResult>& triangulated, size_t offX )
    {
        GFXEngineImage ge( out );
        GFX g( &ge );

        g.color() = Color::GREEN;

        Vector2f p2;

        for( size_t i = 0; i < triangulated.size(); i++ ){
            const DepthInitializer::DepthInitResult& c = triangulated[ i ];
            p2 = c.meas1;
            p2.x += offX;
            g.drawLine( c.meas0, p2 );
        }
    }

}

#endif
