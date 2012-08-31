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
#include <cvt/util/EigenBridge.h>
#include <cvt/util/Signal.h>
#include <cvt/util/CVTAssert.h>

#include <cvt/vision/rgbdvo/RGBDKeyframe.h>

namespace cvt {

    template <class DerivedKF>
    class RGBDVisualOdometry
    {

        public:
            RGBDVisualOdometry( const Matrix3f& K, const VOParams& params );
            ~RGBDVisualOdometry();            

            /**
             *  \brief  update the pose by using the given pose as starting point
             *  \param  pose will be the initial value for the optimization and contains the computed result
             *          it has to be the pose from world to camera!
             */
            void updatePose( Matrix4f& pose, const Image& gray, const Image& depth );

            /**
             *
             */
            void computeAlignment( Matrix4f& pose, const Image& gray, const Image& depth );

            /**
             *  \param kfPose   pose of the keyframe: T_wk
             */
            void addNewKeyframe( const Image& gray, const Image& depth, const Matrix4f& kfPose );

            /**
             *  \brief  get the absolute (world) pose of the last added image
             */
            const Matrix4f&     pose() const;
            size_t              numKeyframes()          const { return _keyframes.size(); }
            const Matrix3f&     intrinsics()            const { return _intrinsics; }

            void setMaxTranslationDistance( float dist )      { _maxTranslationDistance = dist; }
            void setMaxRotationDistance( float dist )         { _maxRotationDistance = Math::deg2Rad( dist ); }
            void setMaxSSD( float dist )                      { _maxSSDSqr = Math::sqr( dist ); }
            void setMinPixelPercentage( float v )             { _minPixPerc = v; }
            void setSelectionPixelPercentage( float v )       { _selectionPixelPercentage = v; }

            void setParams( const VOParams& p )               { _params = p; }
            void setPose( const Matrix4f& pose )              { _currentPose = pose; }

            size_t numOverallKeyframes() const { return _numCreated; }

            float  lastSSD()             const { return _lastResult.costs; }
            size_t lastNumPixels()       const { return _lastResult.numPixels; }
            float  lastPixelPercentage() const { return _lastResult.pixelPercentage * 100.0f; }

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
            Matrix3f                    _intrinsics;
            VOParams                    _params;

            float                       _maxTranslationDistance;
            float                       _maxRotationDistance;
            float                       _maxSSDSqr;
            float                       _minPixPerc;
            float                       _selectionPixelPercentage;

            // current active keyframe
            DerivedKF*                  _activeKeyframe;
            size_t						_numCreated;
            std::vector<DerivedKF>      _keyframes;

            ImagePyramid                _pyramid;
            Matrix4<float>              _currentPose;

            typename DerivedKF::Result  _lastResult;


            bool needNewKeyframe() const;

            void setKeyframeParams( DerivedKF& kf );
    };

    template <class DerivedKF>
    inline RGBDVisualOdometry<DerivedKF>::RGBDVisualOdometry( const Matrix3f& K, const VOParams& params ) :
        _intrinsics( K ),
        _params( params ),
        _maxTranslationDistance( 0.4f ),
        _maxRotationDistance( Math::deg2Rad( 5.0f ) ),
        _maxSSDSqr( Math::sqr( 0.2f ) ),
        _minPixPerc( 0.5f ),
        _selectionPixelPercentage( 0.3f ),
        _activeKeyframe( 0 ),
        _numCreated( 0 ),
        _pyramid( params.octaves, params.pyrScale )
    {
        _currentPose.setIdentity();
    }

    template <class DerivedKF>
    inline RGBDVisualOdometry<DerivedKF>::~RGBDVisualOdometry()
    {
        _activeKeyframe = 0;
        _keyframes.clear();
    }

    template <class DerivedKF>
    inline void RGBDVisualOdometry<DerivedKF>::updatePose( Matrix4f& pose, const Image& gray, const Image& depth )
    {
        _pyramid.update( gray );

        _activeKeyframe->align( _lastResult, pose, _pyramid, depth );

        _currentPose = _lastResult.warp.poseMatrix();

        // check if we need a new keyframe
        if( needNewKeyframe() ){
            addNewKeyframe( gray, depth, _currentPose );
        }

        pose = _currentPose;
    }

    template <class DerivedKF>
    inline void RGBDVisualOdometry<DerivedKF>::computeAlignment( Matrix4f& pose, const Image& gray, const Image& depth )
    {
        _pyramid.update( gray );

        _activeKeyframe->align( _lastResult, pose, _pyramid, depth );

        _currentPose = _lastResult.warp.poseMatrix();

        // check if we need a new keyframe
        if( needNewKeyframe() ){
            addNewKeyframe( gray, depth, _currentPose );
        }

        pose = _currentPose;
    }

    template <class DerivedKF>
    inline void RGBDVisualOdometry<DerivedKF>::addNewKeyframe( const Image& gray, const Image& depth, const Matrix4f& kfPose )
    {
        CVT_ASSERT( ( gray.format()  == IFormat::GRAY_FLOAT ), "Gray image format has to be GRAY_FLOAT" );
        CVT_ASSERT( ( depth.format() == IFormat::GRAY_FLOAT ), "Depth image format has to be GRAY_FLOAT" );

        if( !_activeKeyframe ){
            _keyframes.push_back( DerivedKF( _intrinsics, _params.octaves, _params.pyrScale ) );
            _activeKeyframe = &_keyframes[ 0 ];            
            _currentPose = kfPose;
            _pyramid.update( gray );
        }
        setKeyframeParams( *_activeKeyframe );
        _activeKeyframe->updateOfflineData( kfPose, _pyramid, depth );
        _lastResult.warp.initialize( kfPose );
        _numCreated++;

        // notify observers
        keyframeAdded.notify( _currentPose );
        activeKeyframeChanged.notify();
    }

    template <class DerivedKF>
    inline void RGBDVisualOdometry<DerivedKF>::setKeyframeParams( DerivedKF& kf )
    {
        kf.setDepthMapScaleFactor( _params.depthScale );
        kf.setMinimumDepth( _params.minDepth );
        kf.setGradientThreshold( _params.gradientThreshold );
        kf.setRobustParam( _params.robustParam );
        kf.setMaxIter( _params.maxIters );
        kf.setMinUpdate( _params.minParameterUpdate );
        kf.setSelectionPixelPercentage( _selectionPixelPercentage );
    }

    template <class DerivedKF>
    inline bool RGBDVisualOdometry<DerivedKF>::needNewKeyframe() const
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

        Matrix4f relPose = _currentPose.inverse() * _activeKeyframe->pose();

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

    template <class DerivedKF>
    inline const Matrix4f& RGBDVisualOdometry<DerivedKF>::pose() const
    {
        return _currentPose;
    }

}

#endif // RGBDVISUALODOMETRY_H
