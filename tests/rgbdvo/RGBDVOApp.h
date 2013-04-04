#ifndef CVT_RGBDVOAPP_H
#define CVT_RGBDVOAPP_H

#include <cvt/gui/Application.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/rgbdvo/RGBDKeyframe.h>
#include <cvt/vision/rgbdvo/RobustWeighting.h>
#include <cvt/vision/rgbdvo/RGBDVisualOdometry.h>
#include <cvt/util/ConfigFile.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/util/Delegate.h>
#include <cvt/util/Time.h>

#include <VOGui.h>
#include <VOThread.h>

#include <fstream>

namespace cvt
{
    template <class KFType, class LossFunc>
    class RGBDVOApp
    {
            typedef RGBDVisualOdometry<KFType, LossFunc>    VOType;
            typedef typename KFType::WarpType               WarpType;
            public:
                RGBDVOApp( VOType* vo, const String& folder );
                ~RGBDVOApp();

            private:
                VOGui                       _gui;
                uint32_t                    _timerId;

                VOThread<KFType, LossFunc>  _voThread;
                std::ofstream               _fileOut;

                void setupConnections();
                void gtPoseCb( const Matrix4f& pose );
                void depthCb( const Image& i );
                void colorCb( const Image& i );
                void resultCb( const StepResult& r );
                void writePose( std::ofstream& file, const Matrix4f& pose, double stamp );

                void computeResidualImage( const Image& currGray, const Matrix4f& currPose ) const;

                PointSet3f  _refPts;
                PointSet2f  _refPts2d;


    };

    template <class KFType, class LossFunc>
    inline RGBDVOApp<KFType, LossFunc>::RGBDVOApp( VOType* vo, const String& folder ) :
        _gui( vo->intrinsics(), vo->parameters().depthScale ),
        _voThread( vo, folder )
    {
        setupConnections();

        _gui.setOffsetPose( _voThread.cameraPose() );
        _voThread.run( NULL );
        _fileOut.open( "trajectory.txt" );
    }

    template <class KFType, class LossFunc>
    inline RGBDVOApp<KFType, LossFunc>::~RGBDVOApp()
    {
        Application::unregisterTimer( _timerId );
        _fileOut.close();
    }

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::depthCb( const Image& depth )
    {
        _gui.setCurrentDepth( depth );
    }

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::colorCb( const Image& c )
    {
        _gui.setCurrentRGB( c );
    }

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::gtPoseCb( const Matrix4f& p )
    {
        _gui.setGroundTruthPose( p );
    }

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::resultCb( const StepResult& result )
    {
        _gui.setSSDLabel( result.avgSSDError );
        _gui.setNumPixels( result.numPixels );
        _gui.setPixelPercentage( result.pixelPercentage );
        _gui.setSpeed( result.avgSpeed );
        _gui.setPose( result.pose );

        writePose( _fileOut, result.pose, result.stamp );
    }

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::writePose( std::ofstream &file, const Matrix4f &pose, double stamp )
    {
        Quaterniond q( ( Matrix3d )pose.toMatrix3() );

        file.precision( 15 );
        file << std::fixed << stamp << " "
                 << pose[ 0 ][ 3 ] << " "
                 << pose[ 1 ][ 3 ] << " "
                 << pose[ 2 ][ 3 ] << " "
                 << q.x << " "
                 << q.y << " "
                 << q.z << " "
                 << q.w << std::endl;
    }

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::setupConnections()
    {
        // observe the vo changes:
        Delegate<void ( const KeyframeData& )> kfAddDel( &_gui, &VOGui::addKeyframe );
        Delegate<void ( const Matrix4f& )> gtDel( this, &RGBDVOApp<KFType, LossFunc>::gtPoseCb );
        Delegate<void ( const Image& )> dimgDel( this, &RGBDVOApp<KFType, LossFunc>::depthCb );
        Delegate<void ( const Image& )> cimgDel( this, &RGBDVOApp<KFType, LossFunc>::colorCb );
        Delegate<void ( const StepResult& )> resultDel( this, &RGBDVOApp<KFType, LossFunc>::resultCb );

        _voThread.keyframeSignal.add( kfAddDel );
        _voThread.gtPose.add( gtDel );
        _voThread.currentColor.add( cimgDel );
        _voThread.currentDepth.add( dimgDel );
        _voThread.resultSignal.add( resultDel );

    }

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::computeResidualImage( const Image& /*currGrayf*/, const Matrix4f& /*currPose*/ ) const
    {
//        Matrix4f relPose = currPose.inverse() * _voThread.referencePose();
//        SIMD* simd = SIMD::instance();

//        IMapScoped<const float> gray( currGrayf );

//        Matrix4f projMat( _vo->intrinsics() );
//        projMat *= relPose;
//        std::vector<Vector2f> warpedPts( _refPts.size() );
//        std::vector<float> pixels( _refPts.size() );
//        simd->projectPoints( &warpedPts[ 0 ], projMat, &_refPts[ 0 ], _refPts.size() );
//        simd->warpBilinear1f( &pixels[ 0 ], &warpedPts[ 0 ].x, gray.ptr(), gray.stride(), gray.width(), gray.height(), -1.0f, _refPts.size() );

//        Image res( gray.width(), gray.height(), currGrayf.format() );
//        res.fill( Color::BLACK );

//        IMapScoped<float> map( res );
//        for( size_t i = 0; i < _refPts.size(); i++ ){
//            const float& v = pixels[ i ];
//            if(  v > 0.0f ){
//                const Vector2f& p = _refPts2d[ i ];
//                //std::cout << i << " " << _refPts[ i ] << " -> " << p << std::endl;
//                map( ( int )p.y, ( int )p.x ) = v;
//            }
//        }

//        // save the result
//        Image residual;
//        _activeKFRGB.convert( residual, IFormat::GRAY_FLOAT );

//        residual.save( "ref.png");
//        residual.sub( currGrayf );
//        residual.add( 1.0f );
//        residual.mul( 0.5f );

//        res.save( "warped.png" );
//        residual.save( "residual.png" );
//        currGrayf.save( "current.png" );
//        std::cout << "Dumped Frames" << std::endl;
    }

}

#endif
