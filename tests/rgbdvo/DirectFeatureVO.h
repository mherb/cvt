/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef CVT_DIRECT_FEATURE_VO_H
#define CVT_DIRECT_FEATURE_VO_H

#include <cvt/vision/rgbdvo/KeyframeBase.h>
#include <cvt/vision/FAST.h>
#include <cvt/vision/rgbdvo/IntensityKeyframe.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/vision/rgbdvo/RGBDWarp.h>
#include <cvt/vision/rgbdvo/RobustWeighting.h>
#include <cvt/vision/rgbdvo/Optimizer.h>

namespace cvt {

    class DirectFeatureVO
    {
        public:
            DirectFeatureVO( const Matrix3f &K, const VOParams& params );
            ~DirectFeatureVO();

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


            size_t          numKeyframes()  const                   { return _keyframes.size(); }
            const Matrix3f& intrinsics()    const                   { return _intrinsics; }
            void            setMaxTranslationDistance( float dist ) { _maxTranslationDistance = dist; }
            void            setMaxRotationDistance( float dist )    { _maxRotationDistance = Math::deg2Rad( dist ); }
            void            setMaxSSD( float dist )                 { _maxSSDSqr = Math::sqr( dist ); }
            void            setMinPixelPercentage( float v )        { _minPixPerc = v; }
            void            setSelectionPixelPercentage( float v )  { _selectionPixelPercentage = v; }
            void            setParams( const VOParams& p )          { _params = p; }
            void            setPose( const Matrix4f& pose )         { _cameraPose = pose; }
            const Matrix4f& pose()                const             { return _cameraPose; }
            size_t          numOverallKeyframes() const             { return _numCreated; }
            float           lastSSD()             const             { return _lastResult.costs; }
            size_t          lastNumPixels()       const             { return _lastResult.numPixels; }
            float           lastPixelPercentage() const             { return _lastResult.pixelPercentage * 100.0f; }


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

            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        private:
            typedef StandardWarp<float>         Warp;
            typedef Huber<float>                LossFunc;
            typedef Optimizer<Warp, LossFunc>   OptimizerType;
            typedef IntensityKeyframe<Warp>     Keyframe;

            VOParams                    _params;
            float                       _maxTranslationDistance;
            float                       _maxRotationDistance;
            float                       _maxSSDSqr;
            float                       _minPixPerc;
            float                       _selectionPixelPercentage;
            size_t						_numCreated;
            Keyframe::Result            _lastResult;

            std::vector<Keyframe>   _keyframes;
            Keyframe*               _activeKeyframe;
            OptimizerType           _optimizer;

            // @TODO: paramset!
            Matrix3f        _intrinsics;
            Matrix4f        _cameraPose;
            ImagePyramid    _pyramid;
            FAST            _fastDetector;

            bool needNewKeyframe() const;
            void setKeyframeParams( Keyframe& kf );

    };

    inline DirectFeatureVO::DirectFeatureVO( const Matrix3f &K, const VOParams& params ) :
        _params( params ),
        _maxTranslationDistance( 0.4f ),
        _maxRotationDistance( Math::deg2Rad( 5.0f ) ),
        _maxSSDSqr( Math::sqr( 0.2f ) ),
        _minPixPerc( 0.5f ),
        _selectionPixelPercentage( 0.3f ),
        _numCreated( 0 ),
        _activeKeyframe( 0 ),
        _intrinsics( K ),
        _pyramid( params.octaves, params.pyrScale )

    {
        _optimizer.setMaxIterations( 20 );
        _optimizer.setMinUpdate( 0.00001f );
        _optimizer.setRobustThreshold( 0.1f );
        _fastDetector.setNonMaxSuppress( false );
        _fastDetector.setThreshold( 10 );
    }

    inline DirectFeatureVO::~DirectFeatureVO()
    {
    }

    inline void DirectFeatureVO::updatePose( Matrix4f& pose, const Image& gray, const Image& depth )
    {
        _pyramid.update( gray );

        _optimizer.optimize( _lastResult, pose, *_activeKeyframe, _pyramid, depth );

        _cameraPose = _lastResult.warp.poseMatrix();

        // check if we need a new keyframe
        if( needNewKeyframe() ){
            addNewKeyframe( gray, depth, _cameraPose );
        }

        pose = _cameraPose;
    }

    inline void DirectFeatureVO::addNewKeyframe( const Image& gray, const Image& depth, const Matrix4f& kfPose )
    {
        _pyramid.update( gray );

        if( !_activeKeyframe ){
            // first time here!
            _keyframes.push_back( Keyframe( _intrinsics, _pyramid.octaves(), _pyramid.scaleFactor() ) );
            _activeKeyframe = &_keyframes.back();
            // current pose is the pose of the keyframe!
            _cameraPose = kfPose;
        }

        // Detect FAST features
        std::vector<ScaleFeatures> features( _pyramid.octaves() );
        Image u8;
        for( size_t i = 0; i < _pyramid.octaves(); i++ ){
            VectorVector2Inserter<float> inserter( features[ i ].positions );
            _pyramid[ i ].convert( u8, IFormat::GRAY_UINT8 );
            _fastDetector.extract( u8, inserter );
        }

        setKeyframeParams( *_activeKeyframe );

        // update keyframe using feature positions
        _activeKeyframe->sparseOfflineData( features, kfPose, _pyramid, depth );

        _lastResult.warp.initialize( kfPose );
        _numCreated++;
        // notify observers
        keyframeAdded.notify( _cameraPose );
        activeKeyframeChanged.notify();
    }

    inline bool DirectFeatureVO::needNewKeyframe() const
    {
        // check the ssd:
        float avgSSD = -1.0f;
        if( _lastResult.numPixels )
            avgSSD = _lastResult.costs / _lastResult.numPixels;

        if( _lastResult.pixelPercentage < _minPixPerc ){
            if( avgSSD < _maxSSDSqr ){
                // we only should add a keyframe, if the last SSD was ok
                return true;
            }
        }

        Matrix4f relPose = _cameraPose.inverse() * _activeKeyframe->pose();

        Vector4f t = relPose.col( 3 );
        t[ 3 ] = 0;
        float tmp = t.length();
        if( tmp > _maxTranslationDistance ){
            //std::cout << "Translation Distance: " << tmp << std::endl;
            return true;
        }

        Matrix3f R = relPose.toMatrix3();
        Quaternionf q( R );
        Vector3f euler = q.toEuler();
        tmp = euler.length();
        if( tmp > _maxRotationDistance ){
            //std::cout << "Rotation Distance: " << tmp << std::endl;
            return true;
        }
        return false;
    }

    inline void DirectFeatureVO::setKeyframeParams( Keyframe& kf )
    {
        kf.setDepthMapScaleFactor( _params.depthScale );
        kf.setMinimumDepth( _params.minDepth );
        kf.setGradientThreshold( _params.gradientThreshold );
        kf.setSelectionPixelPercentage( _selectionPixelPercentage );

        _optimizer.setRobustThreshold( _params.robustParam );
        _optimizer.setMaxIterations( _params.maxIters );
        _optimizer.setMinUpdate( _params.minParameterUpdate );
    }

}

#endif
