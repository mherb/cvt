/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <cvt/vision/rgbdvo/RGBDKeyframe.h>
#include <cvt/vision/rgbdvo/SystemBuilder.h>
#include <cvt/vision/rgbdvo/ApproxMedian.h>
#include <Eigen/LU>

namespace cvt {

    class ErrorLogger {
        public:
            ErrorLogger() :
                _maxOctave( 0 ), _maxIteration( 0 )
            {
            }

            ~ErrorLogger()
            {
                if( _data.size() ){
                    saveResult();
                }
            }

            void setGTPose( const Matrix4f& mat )
            {
                _gt = mat.inverse();
            }

            void log( size_t octave, size_t iteration, const Matrix4f& cur )
            {
                Matrix4f rel = _gt * cur;

                float tError = Vector3f( rel.col( 3 ) ).length();
                float rError = rotError( rel );
                //std::cout << octave << " - " << iteration << " - " << rError << " - " << tError << std::endl;
                _data.push_back( LogData( octave, iteration, rError, tError ) );
                _maxOctave = Math::max( _maxOctave, octave );
                _maxIteration = Math::max( _maxIteration, iteration );
            }

        private:
            struct LogData {
                LogData( size_t o, size_t i, float rotE, float tE ):
                    octave( o ), iteration( i ), rError( rotE ), tError( tE )
                {}

                size_t  octave;
                size_t  iteration;
                float   rError;
                float   tError;
            };


            float rotError( const Matrix4f& m )
            {
                return Math::acos( Math::clamp<float>( 0.5f * ( m.toMatrix3().trace() - 1.0f ), -1.0f, 1.0f ) );
            }

            std::vector<LogData>    _data;
            size_t                  _maxOctave;
            size_t                  _maxIteration;

            Matrix4f                _gt;

            struct ScaleResult {
                ScaleResult( size_t maxIters )
                {
                    rotErrorInIteration.resize( maxIters, 0.0f );
                    tErrorInIteration.resize( maxIters, 0.0f );
                    samplesForIteration.resize( maxIters, 0 );
                }

                void add( size_t iter, float rError, float tError )
                {
                    rotErrorInIteration[ iter ] += rError;
                    tErrorInIteration[ iter ] += tError;
                    samplesForIteration[ iter ]++;
                }

                float avgRotErrorForIteration( size_t i ) const
                {
                    size_t n = samplesForIteration[ i ];
                    if( n == 0 )
                        return 0.0f;
                    return rotErrorInIteration[ i ] / n;
                }

                float avgtErrorForIteration( size_t i ) const
                {
                    size_t n = samplesForIteration[ i ];
                    if( n == 0 )
                        return 0.0f;
                    return tErrorInIteration[ i ] / n;
                }

                std::vector<float>  rotErrorInIteration;
                std::vector<float>  tErrorInIteration;
                std::vector<size_t> samplesForIteration;
            };

            void saveResult()
            {
                std::ofstream file;
                file.open( "conv_speed.txt" );
                file.precision( 15 );
                file << "# <octave> <error iter0> <...> <error itern>" << std::endl;

                std::vector<ScaleResult> resultForOctave( _maxOctave + 1, ScaleResult( _maxIteration + 1 ) );
                for( size_t i = 0; i < _data.size(); i++ ){
                    const LogData& d = _data[ i ];
                    resultForOctave[ d.octave ].add( d.iteration, d.rError, d.tError );
                }

                for( size_t k = 0; k < _maxIteration + 1; k++ ){
                    file << k << " ";
                    for( size_t i = 0; i < resultForOctave.size(); i++ ){
                        file << std::fixed << resultForOctave[ i ].avgRotErrorForIteration( k ) << " ";
                        file << std::fixed << resultForOctave[ i ].avgtErrorForIteration( k ) << " ";
                    }
                    file << "\n";
                }

                file.close();
            }
    };

    template <class WarpFunc, class Weighter>
    class Optimizer
    {
        public:
            typedef typename WarpFunc::JacobianType     JacobianType;
            typedef typename WarpFunc::HessianType      HessianType;
            typedef typename WarpFunc::DeltaVectorType  DeltaType;
            typedef typename RGBDKeyframe<WarpFunc>::AlignmentData AlignDataType;

            struct Result {
                EIGEN_MAKE_ALIGNED_OPERATOR_NEW
                Result() :
                    success( false ),
                    numPixels( 0 ),
                    pixelPercentage( 0.0f ),
                    costs( 0.0f )
                {
                }

                bool        success;
                size_t      iterations;
                size_t      numPixels;
                float       pixelPercentage; /* between 0 and 1 */
                float       costs;

                WarpFunc    warp;
            };

            Optimizer();
            virtual ~Optimizer(){}

            void setMaxIterations( size_t iter )    { _maxIter = iter; }
            void setMinUpdate( float v )            { _minUpdate = v; }
            void setMinPixelPercentage( float v )   { _minPixelPercentage = v; }
            void setUseRegularization( bool v )     { _useRegularizer = v; }
            void setRegularizationMatrix( const HessianType& m ) { _regularizer = m; }
            void setRegularizationAlpha( float v )  { _regAlpha = v; }
            void setLogError( bool v )              { _logError = v; }

            /**
             * @brief setCostStopThreshold
             * @param v early stepout, if average costs are below v!
             */
            void setCostStopThreshold( float v )    { _costStopThreshold = v; }

            // scale-space optimization
            void optimize( Result& result,
                           const Matrix4f& posePrediction,
                           RGBDKeyframe<WarpFunc>& reference,
                           const ImagePyramid& grayPyramid,
                           const Image& depthImage );

            void setErrorLoggerGTPose( const Matrix4f& mat ){ _logger.setGTPose( mat ); }

        protected:
            size_t          _maxIter;
            float           _minUpdate;
            float           _minPixelPercentage;
            float           _costStopThreshold;
            bool            _useRegularizer;
            float           _regAlpha;
            HessianType     _regularizer;
            DeltaType       _overallDelta;
            bool            _logError;
            ErrorLogger     _logger;

            Weighter                _weighter;
            SystemBuilder<Weighter> _builder;

            float computeMedian( const float* residuals, size_t n ) const;
            float computeMAD( const float* residuals, size_t n, float median ) const;
            bool checkResult( const Result& res ) const;

            float evaluateSystem( HessianType& hessian, JacobianType& deltaSum,
                                  const JacobianType* jacobians, const float* residuals, size_t n );

            void resetOverallDelta();

        private:
            virtual void optimizeSingleScale( Result& result,
                                              RGBDKeyframe<WarpFunc>& reference,
                                              const Image& gray,
                                              const Image& depthImage,
                                              size_t octave ) = 0;

    };

    template <class WarpFunc, class LossFunc>
    inline Optimizer<WarpFunc, LossFunc>::Optimizer() :
        _maxIter( 10 ),
        _minUpdate( 1e-6 ),
        _minPixelPercentage( 0.8f ),
        _costStopThreshold( 0.002f ),
        _useRegularizer( false ),
        _regAlpha( 0.2f ),
        _regularizer( HessianType::Zero() ),
        _overallDelta( DeltaType::Zero() ),
        _builder( _weighter )
    {
    }

    template <class WarpFunc, class LossFunc>
    inline void Optimizer<WarpFunc, LossFunc>::optimize( Result& result,
                                                         const Matrix4f& posePrediction,
                                                         RGBDKeyframe<WarpFunc> &reference,
                                                         const ImagePyramid& grayPyramid,
                                                         const Image& depthImage )
    {
        Matrix4f tmp4;
        tmp4 = posePrediction.inverse() * reference.pose();

        result.warp.setPose( tmp4 );
        result.costs = 0.0f;
        result.iterations = 0;
        result.numPixels = 0;
        result.pixelPercentage = 0.0f;

        Result saveResult = result;

        if( _useRegularizer ){
            resetOverallDelta();
        }

        reference.updateOnlineData( grayPyramid, depthImage );
        for( int o = grayPyramid.octaves() - 1; o >= 0; o-- ){
            this->optimizeSingleScale( result, reference, grayPyramid[ o ], depthImage, o );

            if( checkResult( result ) ){
                saveResult = result;                
            }
        }

        result = saveResult;

        // convert relative result to absolute pose: TODO: maybe do this outside
        tmp4 = result.warp.pose();
        tmp4 = reference.pose() * tmp4.inverse();
        result.warp.setPose( tmp4 );
    }

    template <class WarpFunc, class LossFunc>
    inline void Optimizer<WarpFunc, LossFunc>::resetOverallDelta()
    {
        _overallDelta.setZero();
    }

    template <class WarpFunc, class LossFunc>
    inline float Optimizer<WarpFunc, LossFunc>::computeMedian( const float* residuals, size_t n ) const
    {
        if( n == 1 ){
            return residuals[ 0 ];
        }

        ApproxMedian medianSelector( 0.0f, 1.0f, 0.02f );

        for( size_t i = 0; i < n; ++i ){
            medianSelector.add( Math::abs( residuals[ i ] ) );
        }

        return medianSelector.approximateNth( n >> 1 );
    }

    template <class WarpFunc, class LossFunc>
    inline float Optimizer<WarpFunc, LossFunc>::computeMAD( const float* residuals, size_t n, float median ) const
    {
        ApproxMedian medianSelector( 0.0f, 0.5f, 0.02f );

        for( size_t i = 0; i < n; ++i ){
            medianSelector.add( Math::abs( residuals[ i ] - median ) );
        }

        return medianSelector.approximateNth( n >> 1 );
    }

    template <class WarpFunc, class LossFunc>
    inline bool Optimizer<WarpFunc, LossFunc>::checkResult( const Result& res ) const
    {
        // too few pixels projected into image
        if( res.pixelPercentage < _minPixelPercentage ){
            //std::cout << "Pixel Percentage: " << res.pixelPercentage << " : " << _minPixelPercentage << std::endl;
            return false;
        }
        return true;
    }


    template <class WarpFunc, class LossFunc>
    inline float Optimizer<WarpFunc, LossFunc>::evaluateSystem( HessianType& hessian, JacobianType& deltaSum,
                                                                const JacobianType* jacobians, const float* residuals, size_t n  )
    {
        float median = this->computeMedian( residuals, n );
        float mad = this->computeMAD( residuals, n, median );

        // this is an estimate for the standard deviation:
        _weighter.setScale( 1.4826f * mad );
        float costs = _builder.build( hessian,
                                      deltaSum,
                                      jacobians,
                                      residuals,
                                      n );
        if( _useRegularizer ){
            float norm = 1.0f / (float) n;
            hessian *= norm;
            deltaSum *= norm;
            hessian.noalias()  += ( 2 * _regAlpha * _regularizer );
            deltaSum.noalias() += ( _regAlpha * _regularizer * _overallDelta ).transpose();
        }

        return costs;
    }

}

#endif // OPTIMIZER_H
