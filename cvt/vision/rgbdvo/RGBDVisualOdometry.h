/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef CVT_RGBDVISUALODOMETRY_H
#define CVT_RGBDVISUALODOMETRY_H

#include <cvt/vision/rgbdvo/KeyframeBase.h>
#include <cvt/gfx/Image.h>
#include <cvt/math/SE3.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/util/Signal.h>
#include <cvt/util/CVTAssert.h>

#include <cvt/vision/rgbdvo/RGBDKeyframe.h>

//#define USE_NEW_VERSION

namespace cvt {

    template <class DerivedKF>
    class RGBDVisualOdometry
    {
        typedef KeyframeBase<DerivedKF> KeyframeType;
        public:
            RGBDVisualOdometry( const Matrix3f& K, const VOParams& params );
            ~RGBDVisualOdometry();

            void updatePose( const Image& gray, const Image& depth );

            /**
             *  \brief  update the pose by using the given pose as starting point
             *  \param  pose will be the initial value for the optimization and contains the computed result
             *          it has to be the pose from world to camera!
             */
            void updatePose( Matrix4f& pose, const Image& gray, const Image& depth );

            /**
             *  \param kfPose   pose of the keyframe: T_wk
             */
            void addNewKeyframe( const Image& gray, const Image& depth, const Matrix4f& kfPose );

            /**
             *  \brief  get the absolute (world) pose of the last added image
             */
            void pose( Matrix4f& pose ) const;

            size_t              numKeyframes()          const { return _keyframes.size(); }
            const KeyframeType& keyframe( size_t idx )  const { return *_keyframes[ idx ]; }
            const KeyframeType& activeKeyframe()        const { return *_activeKeyframe; }
            const Matrix3f&     intrinsics()            const { return _intrinsics; }

            void setMaxTranslationDistance( float dist )      { _maxTranslationDistance = dist; }
            void setMaxRotationDistance( float dist )         { _maxRotationDistance = Math::deg2Rad( dist ); }
            void setMaxSSD( float dist )                      { _maxSSDSqr = Math::sqr( dist ); }

            size_t numOverallKeyframes() const { return _numCreated; }
            const VOResult& lastResult() const { return _lastResult; }

            /******** SIGNALS ************/
            /**
             *  \brief  Signal that will be emitted when a new keyframe was added
             *  \param  Matrix4f will be the pose of the new keyframe in the map
             */
            Signal<const Matrix4f&>    keyframeAdded;

            /**
             *  \brief  Signal that will be emitted when the active keyframe changed
             */
            Signal<void>               activeKeyframeChanged;

        private:
            Matrix3f                    _intrinsics;
            VOParams                    _params;

            float                       _maxTranslationDistance;
            float                       _maxRotationDistance;
            float                       _maxSSDSqr;

            // current active keyframe
            DerivedKF*                  _activeKeyframe;

            size_t						_numCreated;

            /* vector of all the keyframes (TODO: maybe graph would be cooler) */
            std::vector<DerivedKF*>     _keyframes;

            /* current / last pose w.r.t. active keyframe */
            PoseRepresentation          _relativePose;
            VOResult                    _lastResult;

#ifdef USE_NEW_VERSION
            typedef StandardWarp<float>       WarpType;
            //typedef AffineLightingWarp<float>   WarpType;
            typedef RGBDKeyframe<WarpType, Huber<float> >      KType;
            KType                       _keyframeTest;
            Matrix4<float>              _testPose;
            ImagePyramid                _pyramid;
#endif



            bool needNewKeyframe( const VOResult& alignResult ) const;
    };

    template <class DerivedKF>
    inline RGBDVisualOdometry<DerivedKF>::RGBDVisualOdometry( const Matrix3f& K, const VOParams& params ) :
        _intrinsics( K ),
        _params( params ),
        _maxTranslationDistance( 0.4f ),
        _maxRotationDistance( Math::deg2Rad( 5.0f ) ),
        _maxSSDSqr( Math::sqr( 0.2f ) ),
        _activeKeyframe( 0 ),
        _numCreated( 0 )
    #ifdef USE_NEW_VERSION
        ,_keyframeTest( K, params.octaves, params.pyrScale, KType::STORE_RELATIVE ),
        //_keyframeTest( K, params.octaves, params.pyrScale, KType::STORE_ABSOLUTE ),
        _pyramid( params.octaves, params.pyrScale )
  #endif
    {
        #ifdef USE_NEW_VERSION
        _keyframeTest.setDepthMapScaleFactor( params.depthScale );
        _keyframeTest.setMinimumDepth( params.minDepth );
        _keyframeTest.setGradientThreshold( params.gradientThreshold );
        _keyframeTest.setRobustParam( params.robustParam );
        _keyframeTest.setMaxIter( params.maxIters );
        _keyframeTest.setMinUpdate( params.minParameterUpdate );
#endif
    }

    template <class DerivedKF>
    inline RGBDVisualOdometry<DerivedKF>::~RGBDVisualOdometry()
    {
        for( size_t i = 0; i < _keyframes.size(); i++ ){
            delete _keyframes[ i ];
        }
        _activeKeyframe = 0;
        _keyframes.clear();
    }

    template <class DerivedKF>
    inline void RGBDVisualOdometry<DerivedKF>::updatePose( const Image& gray, const Image& depth )
    {
        // align with the current keyframe
        _lastResult = _activeKeyframe->computeRelativePose( _relativePose, gray, _intrinsics, _params );

        #ifdef USE_NEW_VERSION
        KType::Result aResult;
        _pyramid.update( gray );
        _keyframeTest.align( aResult, _testPose, _pyramid );
        _testPose = aResult.warp.poseMatrix();
        #endif

        Matrix4f absPose;
        pose( absPose );
        std::cout << "Current Pose:\n" << absPose << std::endl;

        //std::cout << "New - Old:\n" << ( _testPose - absPose ) << std::endl;

        // check if we need a new keyframe
        if( needNewKeyframe( _lastResult ) ){
            addNewKeyframe( gray, depth, absPose );
            keyframeAdded.notify( absPose );
            activeKeyframeChanged.notify();
        }
    }

    template <class DerivedKF>
    inline void RGBDVisualOdometry<DerivedKF>::updatePose( Matrix4f& pose, const Image& gray, const Image& depth )
    {
        // align with the current keyframe
        // convert pose to a relative prediction:
        const Matrix4f& kfPose = _activeKeyframe->pose();
        pose = pose.inverse() * kfPose;
        Eigen::Matrix4f tmp;
        EigenBridge::toEigen( tmp, pose );
        _relativePose.pose.set( tmp );
        VOResult result = _activeKeyframe->computeRelativePose( _relativePose, gray, _intrinsics, _params );

        // get back the absolute pose;
        this->pose( pose );

        // check if we need a new keyframe
        if( needNewKeyframe( result ) ){
            addNewKeyframe( gray, depth, pose );
            keyframeAdded.notify( pose );
            activeKeyframeChanged.notify();
        }
    }

    template <class DerivedKF>
    inline void RGBDVisualOdometry<DerivedKF>::addNewKeyframe( const Image& gray, const Image& depth, const Matrix4f& kfPose )
    {
        CVT_ASSERT( ( gray.format()  == IFormat::GRAY_FLOAT ), "Gray image format has to be GRAY_FLOAT" );
        CVT_ASSERT( ( depth.format() == IFormat::GRAY_FLOAT ), "Depth image format has to be GRAY_FLOAT" );

        if( _activeKeyframe )
            delete _activeKeyframe;
        _activeKeyframe = KeyframeType::create( gray, depth, kfPose, _intrinsics, _params );

        #ifdef USE_NEW_VERSION
        _pyramid.update( gray );
        static bool first = true;
        if( first ){
            _keyframeTest.updateOfflineData( kfPose, _pyramid, depth );
            _testPose = kfPose;
            first = false;
        } else {
            _keyframeTest.updateOfflineData( _testPose, _pyramid, depth );
        }
        #endif

        // DerivedKF* kf = KeyframeType::create( gray, dFloat, kfPose, _intrinsics, _params );
        //_keyframes.push_back( kf );
        //_activeKeyframe = _keyframes.back();

        // set the relative pose to identity
        SE3<float>::MatrixType I = SE3<float>::MatrixType::Identity();
        _relativePose.pose.set( I );
        _relativePose.bias = 0.0f;
        _relativePose.gain = 0.0f;
        _numCreated++;
    }

    template <class DerivedKF>
    inline bool RGBDVisualOdometry<DerivedKF>::needNewKeyframe( const VOResult& alignResult ) const
    {
        // check the ssd:
        float avgSSD = alignResult.SSD / alignResult.numPixels;
        if( avgSSD > _maxSSDSqr ){
            //std::cout << "Avg SSD: " << avgSSD << std::endl;
            return true;
        }

        const Eigen::Matrix4f& rel = _relativePose.pose.transformation();

        float tmp = rel.block<3, 1>( 0, 3 ).norm();
        if( tmp > _maxTranslationDistance ){
            //std::cout << "Translation Distance: " << tmp << std::endl;
            return true;
        }

        Matrix3f R;
        const Eigen::Matrix3f& RE = rel.block<3, 3>( 0, 0 );
        EigenBridge::toCVT( R, RE );
        Quaternionf q( R );
        Vector3f euler = q.toEuler();
        tmp = euler.length();
        if( tmp > _maxRotationDistance ){
            //std::cout << "Rotation Distance: " << tmp << std::endl;
            return true;
        }
        return false;
    }

    template <class DerivedKF>
    inline void RGBDVisualOdometry<DerivedKF>::pose( Matrix4f& pose ) const
    {
        Matrix4f tmp;
        EigenBridge::toCVT( tmp, _relativePose.pose.transformation() );
        pose = _activeKeyframe->pose() * tmp.inverse();
    }

}

#endif // RGBDVISUALODOMETRY_H
