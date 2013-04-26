#include <ALP.h>

#include <cvt/math/SE3.h>

namespace cvt
{
    ALP3D::ALP3D( const Matrix3f& intrinsics ) :
        _intrinsics( intrinsics ),
        _simd( SIMD::instance() )
    {
        float angleRange = Math::deg2Rad( 1.0f );
        float tRange = 0.1f;
        _parameterRanges.push_back( Vector2f( -angleRange, angleRange ) );
        _parameterRanges.push_back( Vector2f( -angleRange, angleRange ) );
        _parameterRanges.push_back( Vector2f( -angleRange, angleRange ) );
        _parameterRanges.push_back( Vector2f( -tRange, tRange ) );
        _parameterRanges.push_back( Vector2f( -tRange, tRange ) );
        _parameterRanges.push_back( Vector2f( -tRange, tRange ) );
        generateInitialRandomWarps( 3000 );
    }

    ALP3D::~ALP3D()
    {
    }

    void ALP3D::learn( const std::vector<Vector3f>& samplePoints,
                       const std::vector<float>& values,
                       const Image& gray )
    {
        // warped and interpolated pixels
        std::vector<float> sample;
        std::vector<Vector2f> warped;

        size_t nPts = samplePoints.size();
        warped.resize( nPts );
        sample.resize( nPts );

        _intensities = values;
        demeanNormalize( &_intensities[ 0 ], nPts );

        IMapScoped<const float> iMap( gray );

        Matrix4f proj;
        HMat H( nPts, numTransformSamples() );
        for( size_t i = 0; i < _warpMats.size(); i++ ){
            // update the projection matrix
            proj = _intrinsics * _warpMats[ i ];

            // warp the points
            computeIntensities( &sample[ 0 ], warped, samplePoints, proj, iMap );

            demeanNormalize( &sample[ 0 ], nPts );

            // compute the image difference
            computeDifference( &sample[ 0 ], &values[ 0 ], nPts );

            // put it in the i-th col of H
            for( size_t k = 0; k < nPts; k++ ){
                H.coeffRef( i, k ) = sample[ k ];
            }
        }

        // TODO: add some noise to H or r?
        // now use the training data and the Y-Matrix to compute the predictor for these points
        // A = Y H^T (HH^T)^-1
        LPMat pred( 6, nPts );
        _predictor.resize( 6, nPts );
        pred = _warps * H.transpose() * ( H * H.transpose() ).inverse();

        // combine this with the already learned predictor
        _predictor = pred;
        _currentPoints3d = samplePoints;
    }

    void ALP3D::track( Matrix4f& posePred, const Image& grayf )
    {
        SE3<float> pose;
        pose.set( posePred );

        size_t iter = 0;

        size_t np = _currentPoints3d.size();

        _lastWarped.resize( np );

        IMapScoped<const float> iMap( grayf );

        cvt::Matrix4f currT, proj;
        Eigen::Matrix<float, Eigen::Dynamic, 1> warpedPixels( np, 1 );
        PoseVec inc;
        float* pData = warpedPixels.data();
        while( iter < 5 ){
            // get current pose
            EigenBridge::toCVT( currT, pose.transformation() );

            // update the projection matrix
            proj = _intrinsics * currT;

            // extract the pixels
            computeIntensities( pData, _lastWarped, _currentPoints3d, proj, iMap );

            // normalize pixels
            demeanNormalize( pData, np );

            // compute residuals
            computeDifference( pData, &_intensities[ 0 ], np );

            // compute increment using the LP
            inc = _predictor * warpedPixels;

            std::cout << inc << std::endl;

            // apply increment to current pose
            pose.apply( inc );

            ++iter;
        }

        EigenBridge::toCVT( posePred, pose.transformation() );
    }

    void ALP3D::demeanNormalize( float* v, size_t n ) const
    {
        float mean = 0.0f;
        float sigma = 0.0f;
        for( size_t i = 0; i < n; i++ ){
            mean += v[ i ];
            sigma += Math::sqr( v[ i ] );
        }
        mean /= n;
        sigma /= n;

        sigma = Math::sqrt( sigma - Math::sqr( mean ) );

        // -mean / sigma
        float invSigma = 1.0f / sigma;
        for( size_t i = 0; i < n; i++ ){
            v[ i ] -= mean;
            v[ i ] *= invSigma;
        }
    }

    void ALP3D::computeDifference( float* normalizedWarped,
                                   const float* sampleValues,
                                   size_t np ) const
    {
        SIMD::instance()->Sub( normalizedWarped, normalizedWarped, &sampleValues[ 0 ], np );
    }

    void ALP3D::generateInitialRandomWarps( size_t n )
    {
        _warps.resize( 6, n );
        _warpMats.resize( n );
        float* data = _warps.data();

        Eigen::Matrix4f id( Eigen::Matrix4f::Identity() );
        SE3<float> pose;
        for( size_t i = 0; i < n; i++ ){
            // generate a random pose increment
            genRandomPoseVec( data + i * 6 );

            // get Eigen Vector representation
            Eigen::Map<PoseVec> m( data+i );

            // apply the increment to the identity pose
            pose.apply( m );

            // store the transformation matrix of the incremental pose
            EigenBridge::toCVT( _warpMats[ i ], pose.transformation() );

            // reset the pose to identity
            pose.set( id );
        }
    }

    void ALP3D::genRandomPoseVec( float* column )
    {
        for( size_t i = 0; i < 6; i++ ){
            column[ i ] = Math::rand( _parameterRanges[ i ].x, _parameterRanges[ i ].y );
        }
    }

    void ALP3D::computeIntensities( float* intensities,
                                    std::vector<Vector2f>& warped,
                                    const std::vector<Vector3f>& pts3d,
                                    const Matrix4f& proj,
                                    const IMapScoped<const float>& iMap ) const
    {
        _simd->projectPoints( &warped[ 0 ], proj, &pts3d[ 0 ], pts3d.size() );
        _simd->warpBilinear1f( intensities, &warped[ 0 ].x, iMap.ptr(), iMap.stride(), iMap.width(), iMap.height(), 0.0f, pts3d.size() );
    }
}
