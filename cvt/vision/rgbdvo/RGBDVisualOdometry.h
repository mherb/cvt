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

#include <cvt/gfx/Image.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/util/Signal.h>
#include <cvt/util/CVTAssert.h>
#include <cvt/util/ConfigFile.h>

#include <cvt/vision/rgbdvo/RGBDKeyframe.h>
#include <cvt/vision/rgbdvo/Optimizer.h>
#include <cvt/vision/rgbdvo/LMOptimizer.h>
#include <cvt/vision/rgbdvo/SplittedOptimizer.h>
#include <cvt/vision/rgbdvo/TROptimizer.h>

namespace cvt {

    template <class KFType, class LossFunction>
    class RGBDVisualOdometry
    {
        public:
            struct Params {
                Params() :
                    pyrOctaves( 3 ),
                    pyrScale( 0.5f ),
                    maxTranslationDistance( 0.4f ),
                    maxRotationDistance( Math::deg2Rad( 5.0f ) ),
                    maxSSDSqr( Math::sqr( 0.2f ) ),
                    minPixelPercentage( 0.3f ),
                    autoReferenceUpdate( true ),
                    propagateDepth( true ),
                    depthScale( 1000.0f ),
                    minDepth( 0.5f ),
                    gradientThreshold( 0.02f ),
                    robustThreshold( 0.5f ),
                    maxIters( 10 ),
                    minParameterUpdate( 1e-6 )
                {}

                Params( ConfigFile& cfg ) :
                    pyrOctaves( cfg.valueForName<int>( "pyrOctaves", 3 ) ),
                    pyrScale( cfg.valueForName<float>( "pyrScale", 0.5f ) ),
                    maxTranslationDistance( cfg.valueForName<float>( "maxTranslationDistance", 0.4f ) ),
                    maxRotationDistance( Math::deg2Rad( cfg.valueForName<float>( "maxRotationDistance", 5.0f ) ) ),
                    maxSSDSqr( Math::sqr( cfg.valueForName<float>( "maxSSD", 0.2f ) ) ),
                    minPixelPercentage( cfg.valueForName<float>( "minPixelPercentage", 0.3f ) ),
                    autoReferenceUpdate( cfg.valueForName<bool>( "autoReferenceUpdate", true ) ),
                    propagateDepth( cfg.valueForName<bool>( "propagateDepth", true ) ),
                    depthScale( cfg.valueForName<float>( "depthFactor", 1000.0f ) *
                                cfg.valueForName<float>( "depthScale", 1.0f ) ),
                    minDepth( cfg.valueForName<float>( "minDepth", 0.5f ) ),
                    gradientThreshold( cfg.valueForName<float>( "gradientThreshold", 0.02f ) ),
                    robustThreshold( cfg.valueForName<float>( "robustThreshold", 0.5f ) ),
                    maxIters( cfg.valueForName<int>( "maxIters", 10 ) ),
                    minParameterUpdate( cfg.valueForName<float>( "minParameterUpdate", 1e-6f ) )
                {
                    // TODO: Params should become a parameterset
                    // conversion between paramset and configfile!
                }

               // pyramid
               size_t pyrOctaves;
               float  pyrScale;

               // keyframe recreation thresholds
               float maxTranslationDistance;
               float maxRotationDistance;
               float maxSSDSqr;
               float minPixelPercentage;

               // automatic update of reference when needed
               bool autoReferenceUpdate;
               // propagate depth on keyframe update
               bool propagateDepth;

               // how many pixels are one meter
               // d_meters = d_pix / depthScale
               float depthScale;
               float minDepth;
               float gradientThreshold;

               // optimizer:
               float    robustThreshold;
               size_t   maxIters;
               float    minParameterUpdate;
            };

            RGBDVisualOdometry( const Matrix3f& K, const Params& params );
            ~RGBDVisualOdometry();

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
            const Matrix4f& pose() const;

            /**
             *  \brief add additional points to the reference frame
             */
            void addFeaturesToKeyframe( const std::vector<Vector3f>& pts );

            size_t          numKeyframes()  const                   { return _keyframes.size(); }
            const Matrix3f& intrinsics()    const                   { return _intrinsics; }
            void            setPose( const Matrix4f& pose )         { _currentPose = pose; }
            size_t          numOverallKeyframes() const             { return _numCreated; }
            float           lastSSD()             const             { return _lastResult.costs; }
            size_t          lastNumPixels()       const             { return _lastResult.numPixels; }
            float           lastPixelPercentage() const             { return _lastResult.pixelPercentage * 100.0f; }
            void            setParameters( const Params& p )        { _params = p; }

            /******** SIGNALS ************/
            /**
             *  \brief  Signal that will be emitted when a new keyframe was added
             *  \param  Matrix4f will be the pose of the new keyframe in the map
             */
            Signal<const Matrix4f&>    keyframeAdded;

            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        private:
            typedef typename KFType::WarpType    Warp;
            typedef typename RGBDKeyframe<Warp>::AlignmentData   AlignDataType;
            //typedef LMOptimizer<WFunc, LossFunction> OptimizerType;
            //typedef SplittedOptimizer<WFunc, LossFunction> OptimizerType;
            typedef Optimizer<Warp, LossFunction> OptimizerType;
            typedef typename OptimizerType::Result  Result;
            //typedef TROptimizer<WFunc, LossFunction> OptimizerType;

            Params                      _params;

            OptimizerType               _optimizer;
            Matrix3f                    _intrinsics;

            // current active keyframe
            KFType*                     _activeKeyframe;
            size_t                      _numCreated;
            std::vector<KFType>         _keyframes;

            ImagePyramid                _pyramid;
            Matrix4<float>              _currentPose;

            Result                      _lastResult;

            bool needNewKeyframe() const;
            void setKeyframeParams( KFType& kf );


            //std::vector<ScaleFeatures>  _gridForScale;
            //void generateFeatureGrid( std::vector<Vector2f>& features, size_t width, size_t height, size_t nx, size_t ny );

            void propagateDepth( Image& depth, const Matrix4f& pose ) const;
    };

    template <class KFType, class LossFunction>
    inline RGBDVisualOdometry<KFType, LossFunction>::RGBDVisualOdometry( const Matrix3f& K, const Params& p ) :
        _params( p ),
        _intrinsics( K ),
        _activeKeyframe( 0 ),
        _numCreated( 0 ),
        _pyramid( p.pyrOctaves, p.pyrScale )
    {
        _currentPose.setIdentity();
    }

    template <class DerivedKF, class LossFunction>
    inline RGBDVisualOdometry<DerivedKF, LossFunction>::~RGBDVisualOdometry()
    {
        _activeKeyframe = 0;
        _keyframes.clear();
    }

    template <class DerivedKF, class LossFunction>
    inline void RGBDVisualOdometry<DerivedKF, LossFunction>::updatePose( Matrix4f& pose, const Image& gray, const Image& depth )
    {
        _pyramid.update( gray );

        _optimizer.optimize( _lastResult, pose, *_activeKeyframe, _pyramid, depth );

        _currentPose = _lastResult.warp.pose();

        // check if we need a new keyframe
        if( _params.autoReferenceUpdate && needNewKeyframe() ){
            addNewKeyframe( gray, depth, _currentPose );
        }

        pose = _currentPose;
    }

    template <class KFType, class LossFunction>
    inline void RGBDVisualOdometry<KFType, LossFunction>::addNewKeyframe( const Image& gray, const Image& depth, const Matrix4f& kfPose )
    {
        CVT_ASSERT( ( gray.format()  == IFormat::GRAY_FLOAT ), "Gray image format has to be GRAY_FLOAT" );
        CVT_ASSERT( ( depth.format() == IFormat::GRAY_FLOAT ), "Depth image format has to be GRAY_FLOAT" );

        _currentPose = kfPose;
        Image dCopy( depth );
        // for the moment, only one keyframe
        if( !_activeKeyframe ){
            _keyframes.push_back( KFType( _intrinsics, _pyramid.octaves(), _pyramid.scaleFactor() ) );
            _activeKeyframe = &_keyframes[ 0 ];
            // _pyramid only needs to be updated if its the first keyframe?! -> this is ugly!
            _pyramid.update( gray );
        } else if( _params.propagateDepth ){
            propagateDepth( dCopy, kfPose );
        }

        setKeyframeParams( *_activeKeyframe );
        _activeKeyframe->updateOfflineData( kfPose, _pyramid, dCopy );
        _lastResult.warp.initialize( kfPose );
        _numCreated++;

        // testing:
        // dCopy.sub( depth );
        // dCopy.save( "propagated_depth.png" );

        // notify observers
        keyframeAdded.notify( _currentPose );
    }

    template <class KFType, class LossFunction>
    inline void RGBDVisualOdometry<KFType, LossFunction>::setKeyframeParams( KFType &kf )
    {
        // TODO: introduce keyframe parameters
        kf.setDepthMapScaleFactor( _params.depthScale );
        kf.setMinimumDepth( _params.minDepth );
        kf.setGradientThreshold( _params.gradientThreshold );

        _optimizer.setRobustThreshold( _params.robustThreshold );
        _optimizer.setMaxIterations( _params.maxIters );
        _optimizer.setMinUpdate( _params.minParameterUpdate );
        _optimizer.setMinPixelPercentage( _params.minPixelPercentage );
    }

    template <class DerivedKF, class LossFunction>
    inline bool RGBDVisualOdometry<DerivedKF, LossFunction>::needNewKeyframe() const
    {
        // check the ssd:
        float avgSSD = -1.0f;
        if( _lastResult.numPixels )
            avgSSD = _lastResult.costs / _lastResult.numPixels;

        if( _lastResult.pixelPercentage < _params.minPixelPercentage ){
            if( avgSSD < _params.maxSSDSqr ){
                // we only should add a keyframe, if the last SSD was ok
                return true;
            }
        }

        Matrix4f relPose = _currentPose.inverse() * _activeKeyframe->pose();

        Vector4f t = relPose.col( 3 );
        t[ 3 ] = 0;
        float tmp = t.length();
        if( tmp > _params.maxTranslationDistance ){
            //std::cout << "Translation Distance: " << tmp << std::endl;
            return true;
        }

        Matrix3f R = relPose.toMatrix3();
        Quaternionf q( R );
        Vector3f euler = q.toEuler();
        tmp = euler.length();
        if( tmp > _params.maxRotationDistance ){
            //std::cout << "Rotation Distance: " << tmp << std::endl;
            return true;
        }
        return false;
    }

    template <class DerivedKF, class LossFunction>
    inline const Matrix4f& RGBDVisualOdometry<DerivedKF, LossFunction>::pose() const
    {
        return _currentPose;
    }

    template <class DerivedKF, class LossFunction>
    inline void RGBDVisualOdometry<DerivedKF, LossFunction>::propagateDepth( Image& depth, const Matrix4f& pose ) const
    {
        // project the points of the current keyframe onto the depth image
        if( _activeKeyframe ){
            const AlignDataType& d = _activeKeyframe->dataForScale( 0 );

            IMapScoped<float> dMap( depth );

            // pose is T_W_C
            // keyframe pose is T_W_K
            // we need T_C_K to transform points from K to C
            // T_C_K = T_W_C^-1 * T_W_K
            Matrix4f relativePose = pose.inverse() * _activeKeyframe->pose();

            // tranform points to the other frame
            std::vector<Vector3f> transformedPts( d.size() );
            SIMD* simd = SIMD::instance();
            simd->transformPoints( &transformedPts[ 0 ], relativePose, &d.points()[ 0 ], transformedPts.size() );

            for( size_t i = 0; i < transformedPts.size(); i++ ){
                Vector2f uv = _intrinsics * transformedPts[ i ];
                if( uv.x >= 0 && uv.x < depth.width() &&
                    uv.y >= 0 && uv.y < depth.height() ){
                    // scale the Z value:
                   int x = uv.x;
                   int y = uv.y;
                   float& dpos = dMap( y, x );
                   if( dpos > 1 ){
                       // average the Z value:
                       dpos = 0.5f * ( dpos + transformedPts[ i ].z * _params.depthScale / ( float )0xFFFF );
                   } else {
                       dpos = transformedPts[ i ].z * _params.depthScale / ( float )0xFFFF;
                   }
                }
            }
        }
    }

}

#endif // RGBDVISUALODOMETRY_H
