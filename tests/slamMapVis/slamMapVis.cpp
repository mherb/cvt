#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/StereoCameraCalibration.h>
#include <cvt/io/RGBDParser.h>

#include <cvt/gfx/ifilter/IWarp.h>

#include <cvt/vision/slam/SlamMap.h>

#include "GLSceneView.h"

using namespace cvt;

void initStereoCalib( StereoCameraCalibration& stereoCalib,
                      Image& undist0, Image& undist1,
                      const CameraCalibration& c0,
                      const CameraCalibration& c1,
                      size_t w, size_t h )
{
    StereoCameraCalibration scam( c0, c1 );
    undist0.reallocate( w, h, IFormat::GRAYALPHA_FLOAT );
    undist1.reallocate( w, h, IFormat::GRAYALPHA_FLOAT );
    scam.undistortRectify( stereoCalib, undist0, undist1, w, h, false );
}

void pixelValueForPosition( Vector4f& color, const float* ptr, size_t fstride, const Vector2f& pos )
{
    const float* pixel = ptr + ( int )( pos.y ) * fstride + ( int )pos.x * 4;
    color.x = pixel[ 0 ];
    color.y = pixel[ 1 ];
    color.z = pixel[ 2 ];
    color.w = pixel[ 3 ];
}

int main( int argc, char** argv )
{
    if( argc < 2 ){
        std::cout << "Usage: " << argv[ 0 ] << " <folder>" << std::endl;
        return 0;
    }

    float pointSize = 1.0f;

    if( argc > 2 ){
        String arg( argv[ 2 ] );
        pointSize = arg.toFloat();
    }


    try {
        String folder( argv[ 1 ] );


        String mapFile( folder );
        mapFile += "/map.xml";

        String calibFile0( folder );
        calibFile0 += "/ueye_4002738788.xml";
        String calibFile1( folder );
        calibFile1 += "/ueye_4002738791.xml";

        StereoCameraCalibration sCalib;
        Image undist0, undist1;

        {
            CameraCalibration c0, c1;
            std::cout << calibFile0 << std::endl;
            std::cout << calibFile1 << std::endl;
            c0.load( calibFile0 );
            c1.load( calibFile1 );
            initStereoCalib( sCalib, undist0, undist1, c0, c1, 752, 480 );
        }

        SlamMap map;
        std::cout << mapFile << std::endl;
        map.load( mapFile );

        ScenePoints pts( "Points" );


        String iFile;
        Image image, icolor8, icolor, undistorted;
        for( int i = 0; i < map.numKeyframes(); i++) {
            ScenePoints ptsnew( "BLA" );

            // load the image:
            iFile.sprintf( "%s/ueye_4002738788_keyframe_%05d.cvtraw", folder.c_str(), i );
            std::cout << "Loading image: " << iFile << std::endl;

            if( !FileSystem::exists( iFile ) )
                std::cout << "File not found!" << std::endl;


            image.load( iFile );
            std::cout << "Converting to color" << std::endl;
            image.convert( icolor8, IFormat::RGBA_UINT8 );
            icolor8.convert( icolor, IFormat::RGBA_FLOAT );

            std::cout << "Undistortion" << std::endl;
            // undistort it:
            IWarp::apply( undistorted, icolor, undist0 );

            const Keyframe& kf = map.keyframeForId( i );
            IMapScoped<float> imageMap( undistorted );
            Vector2f pos;

            std::vector<Vector3f> vertices;
            std::vector<Vector4f> colors;

            for( size_t p = 0; p < map.numFeatures(); p++ ){
                const MapFeature& mf = map.featureForId( p );

                if( mf.visibleInCamera( i ) ){
                    const Eigen::Vector4d& p3d = mf.estimate();
                    const MapMeasurement& mm = kf.measurementForId( p );

                    pos.x = mm.point[ 0 ];
                    pos.y = mm.point[ 1 ];

                    vertices.push_back( Vector3f( p3d[ 0 ], p3d[ 1 ], p3d[ 2 ] ) );

                    // get pixel value at position
                    Vector4f c;
                    pixelValueForPosition( c, imageMap.ptr(), imageMap.stride() / sizeof( float), pos ) ;
                    colors.push_back( c );
                }
            }

            ptsnew.setVerticesWithColor( &vertices[ 0 ], &colors[ 0 ], vertices.size() );
            pts.add( ptsnew );
        }

        std::cout << pts.boundingBox() << std::endl;

        pts.setPointSize( pointSize );

        Window w( "meshView" );
        w.setSize( 640, 480 );
        GLSceneView view( pts );

        WidgetLayout wl;
        wl.setAnchoredTopBottom( 0, 0 );
        wl.setAnchoredLeftRight( 0, 0 );
        w.addWidget( &view, wl );
        w.setVisible( true );

        w.update();

        Application::run();

    } catch ( cvt::Exception& e ){
        std::cout << e.what() << std::endl;
    }

    return 0;

}
