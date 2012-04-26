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

#include <KeyframeBase.h>
#include <cvt/gfx/Image.h>
#include <cvt/math/SE3.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/util/Signal.h>

namespace cvt {

    template <class DerivedKF>
    class RGBDVisualOdometry
    {
        typedef KeyframeBase<DerivedKF> KeyframeType;
        public:
            RGBDVisualOdometry( const Matrix3f& K, const VOParams& params );
            ~RGBDVisualOdometry();

            void updatePose( const Image& gray, const Image& depth );
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

            /**
             *  \brief  Signal that will be emitted when a new keyframe was added
             *  \param  Matrix4f will be the pose of the new keyframe in the map
             */
            Signal<const Matrix4f&>    keyframeAdded;

            /**
             *  \brief  Signal that will be emitted when a the active keyframe changed
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

            /* vector of all the keyframes (TODO: maybe graph would be cooler) */
            std::vector<DerivedKF*>  _keyframes;

            /* current / last pose w.r.t. active keyframe */
            SE3<float>                  _relativePose;

            bool needNewKeyframe( const VOResult& alignResult ) const;
    };

    template <class DerivedKF>
    inline RGBDVisualOdometry<DerivedKF>::RGBDVisualOdometry( const Matrix3f& K, const VOParams& params ) :
        _intrinsics( K ),
        _params( params ),
        _maxTranslationDistance( 0.4f ),
        _maxRotationDistance( Math::deg2Rad( 5.0f ) ),
        _maxSSDSqr( Math::sqr( 0.2f ) ),
        _activeKeyframe( 0 )
    {
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
        VOResult result = _activeKeyframe->computeRelativePose( _relativePose, gray, _intrinsics, _params );

        // check if we need a new keyframe
        if( needNewKeyframe( result ) ){
            // TODO: check if there is a close enough keyframe somewhere in the map
            // if so, use that one, update the relative pose accordingly and notify activeKeyframe observers

            Matrix4f absPose;
            pose( absPose );
            addNewKeyframe( gray, depth, absPose );
            keyframeAdded.notify( absPose );
            activeKeyframeChanged.notify();
        }
    }

    template <class DerivedKF>
    inline void RGBDVisualOdometry<DerivedKF>::addNewKeyframe( const Image& gray, const Image& depth, const Matrix4f& kfPose )
    {
        Image dFloat( depth.width(), depth.height(), IFormat::GRAY_FLOAT );
        depth.convert( dFloat );

        if( _activeKeyframe )
            delete _activeKeyframe;
        _activeKeyframe = KeyframeType::create( gray, dFloat, kfPose, _intrinsics, _params );

        // DerivedKF* kf = KeyframeType::create( gray, dFloat, kfPose, _intrinsics, _params );
        //_keyframes.push_back( kf );
        //_activeKeyframe = _keyframes.back();

        // set the relative pose to identity
        SE3<float>::MatrixType I = SE3<float>::MatrixType::Identity();
        _relativePose.set( I );
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

        const Eigen::Matrix4f& rel = _relativePose.transformation();

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
        EigenBridge::toCVT( tmp, _relativePose.transformation() );
        pose = _activeKeyframe->pose() * tmp.inverse();
    }
}

#endif // RGBDVISUALODOMETRY_H
