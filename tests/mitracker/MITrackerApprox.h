#ifndef MITRACKER_H
#define MITRACKER_H

#include <cvt/gfx/Image.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/gfx/IKernel.h>
#include <cvt/gfx/ifilter/ITransform.h>
#include <cvt/math/BSpline.h>
#include <cvt/gfx/IHistogram.h>

#include "PoseHomography.h"
#include "PoseTranslation.h"

#include <cvt/math/SL3.h>
#include <cvt/math/Translation2D.h>
#include <cvt/math/Sim2.h>
#include <cvt/math/GA2.h>

#define NUMPARAMS 8

namespace cvt {
    class MITracker {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW

            typedef SL3<float> PoseType;
            typedef Eigen::Matrix<float,PoseType::NPARAMS, 1> JacType;
            typedef Eigen::Matrix<float,PoseType::NPARAMS, PoseType::NPARAMS> HessType;

            MITracker();
            ~MITracker();

            void	updateInput( const Image& img );

            /**
             *	@param	img	the current image (grayscale)
             *	@param	pos	the position of the template in the current image
             */
            void	updateTemplate( const Image& img, const Vector2f& pos );

            const Matrix3f& pose();
            void setPose( const Matrix3f & hom );

            const Image&	templateImage() const { return _itemplate; }
            const Image&	templateGradX() const { return _templateGradX; }
            const Image&	templateGradY() const { return _templateGradY; }
            const Image&	warped()		const { return _warped; }

            size_t	maxIterations()			const { return _maxIter; }
            void	setMaxIterations( size_t m )  { _maxIter = m; }

            float	MI();

            const JacType&  jacobian() { return _miJacobian; }
            const HessType& hessian()  { return _miHessian; }

        private:
            void	updateInputHistograms();
            void	updateTemplateHistogram();
            void	updateDerivatives();
            void	updateTemplateGradients();
            void	offlineTemplateDerivatives();
            void	offlineTemplateHessian();

            void	updateApproxHessian();

            float	solveDeltaPose();

            // Histogram related
            size_t	_numBins;

            // Derivatives / Histograms
            Image	_itemplate;
            Image	_templateGradX;
            Image	_templateGradY;
            Image	_templateGradXX;
            Image	_templateGradYY;
            Image	_templateGradXY;
            //			IHistogramf _templateHist;

            // Joint histogram
            float*	_jhist;
            float*	_thist;

            // backprojection of image to template space
            Image	_warped;

            PoseType		_pose;

            JacType			_miJacobian;
            HessType        _miHessian;

            // for the template we can calculate offline data once:
            JacType*            _jTemp;
            HessType*           _hTemp;

            float*              _tempSplineWeights;
            float*              _tempSplineDerivWeights;
            std::vector<int>    _binValues;

            size_t              _maxIter;

            Matrix3f            _currPose;

            float               _gradThresh;
            bool*               _evaluated;
    };

    inline MITracker::MITracker() :
        _numBins( 16 ),
        _jTemp( 0 ),        
        _hTemp( 0 ),
        _tempSplineWeights( 0 ),
        _tempSplineDerivWeights( 0 ),
        _maxIter( 10 ),
        _gradThresh( 0.04f ),
        _evaluated( 0 )
    {
        _jhist = new float[ ( _numBins + 1 ) * ( _numBins + 1 ) ];
        _thist = new float[ ( _numBins + 1 ) ];
    }

    inline MITracker::~MITracker()
    {
        delete[] _jhist;
        delete[] _thist;
        if( _jTemp )
            delete[] _jTemp;        
        if( _hTemp )
            delete[] _hTemp;
        if( _tempSplineWeights )
            delete[] _tempSplineWeights;
        if( _tempSplineDerivWeights )
            delete[] _tempSplineDerivWeights;
        if( _evaluated )
            delete[] _evaluated;
    }

    inline void MITracker::updateInput( const Image & _img )
    {
        size_t iter = 0;
        Matrix3f hinv;
        Image img, tmp;
        _img.convert( img, IFormat::GRAY_FLOAT );
        tmp.reallocate( _warped );

        while( iter < _maxIter ){
            // warp back according to current pose
            const Matrix3f & h = pose();
            hinv = h.inverse();

            tmp.fill( Color::WHITE );
            ITransform::apply( tmp, img, hinv, _warped.width(), _warped.height() );

            tmp.convolve( _warped, IKernel::GAUSS_HORIZONTAL_5, IKernel::GAUSS_VERTICAL_5 );

            // calculate the online stuff:
            updateInputHistograms();
            updateDerivatives();

            float epsilon = solveDeltaPose();

            // step out due to small update
            if( epsilon < 1e-4 )
                return;

            iter++;
        }
    }

    inline float MITracker::solveDeltaPose()
    {
        // calc the update:
        Eigen::Matrix<float, NUMPARAMS, 1> delta;
        delta.setZero();
        delta = -_miHessian * _miJacobian;
        _pose.applyInverse( -delta );
        return delta.norm();
    }

    inline void MITracker::updateTemplateGradients()
    {
        _templateGradX.reallocate( _itemplate.width(), _itemplate.height(), _itemplate.format() );
        _templateGradY.reallocate( _itemplate.width(), _itemplate.height(), _itemplate.format() );
        _templateGradXX.reallocate( _itemplate.width(), _itemplate.height(), _itemplate.format() );
        _templateGradYY.reallocate( _itemplate.width(), _itemplate.height(), _itemplate.format() );
        _templateGradXY.reallocate( _itemplate.width(), _itemplate.height(), _itemplate.format() );

        IKernel kx = IKernel::HAAR_HORIZONTAL_3;
        IKernel ky = IKernel::HAAR_VERTICAL_3;
        kx.scale( -0.5f );
        ky.scale( -0.5f );
        IKernel laplx = IKernel::LAPLACE_5_XX;
        laplx.scale( -0.25f );
        IKernel laply = IKernel::LAPLACE_5_YY;
        laply.scale( -0.25f );

        _itemplate.convolve( _templateGradX, kx );
        _itemplate.convolve( _templateGradY, ky );
        _itemplate.convolve( _templateGradXX, laplx );
        _itemplate.convolve( _templateGradYY, laply );


        float kxydata[]={0.25f, 0.0f, -0.25,
                         0.0f, 0.0f, 0.0f,
                         -0.25f, 0.0f, 0.25f };
        IKernel kxy( 3, 3, kxydata );
        //_itemplate.convolve( _templateGradXY, kx, ky );
        _itemplate.convolve( _templateGradXY, kxy );
    }

    inline void MITracker::updateTemplate( const Image& img, const Vector2f& pos )
    {
        Time t;
        Matrix3<float> mat;
        mat.setIdentity();
        mat[ 0 ][ 2 ] = pos.x;
        mat[ 1 ][ 2 ] = pos.y;
        _pose.set( mat );

        Image tmp;
        img.convert( tmp, IFormat::GRAY_FLOAT );
        _itemplate.reallocate( tmp );
        tmp.convolve( _itemplate, IKernel::GAUSS_HORIZONTAL_5, IKernel::GAUSS_VERTICAL_5 );

        updateTemplateGradients();

        offlineTemplateDerivatives();
        updateTemplateHistogram();

        // reallocate the backwarped sizes
        _warped.reallocate( _itemplate );

        updateApproxHessian();

        std::cout << "Template update took: " << t.elapsedMilliSeconds() << std::endl;
    }

    inline const Matrix3f& MITracker::pose()
    {
        //return _pose.pose();
        const Eigen::Matrix<float, 3, 3> & h = _pose.transformation();
        _currPose[ 0 ][ 0 ] = h( 0, 0 );
        _currPose[ 0 ][ 1 ] = h( 0, 1 );
        _currPose[ 0 ][ 2 ] = h( 0, 2 );
        _currPose[ 1 ][ 0 ] = h( 1, 0 );
        _currPose[ 1 ][ 1 ] = h( 1, 1 );
        _currPose[ 1 ][ 2 ] = h( 1, 2 );
        _currPose[ 2 ][ 0 ] = h( 2, 0 );
        _currPose[ 2 ][ 1 ] = h( 2, 1 );
        _currPose[ 2 ][ 2 ] = h( 2, 2 );
        return _currPose;
    }

    inline float MITracker::MI()
    {
        float mi = 0;
        float _thist[ _numBins + 1 ];
        float _whist[ _numBins + 1 ];

        for( size_t x = 0; x < _numBins+1; x++ ) {
            _thist[ x ] = 0;
            _whist[ x ] = 0;
        }
        for( size_t y = 0; y < _numBins+1; y++ ) {
            for( size_t x = 0; x < _numBins+1; x++ ) {
                _thist[ y ] += _jhist[ y * ( _numBins + 1 ) + x ];
                _whist[ x ] += _jhist[ y * ( _numBins + 1 ) + x ];
            }
        }

        for( size_t y = 0; y < _numBins+1; y++ ) {
            for( size_t x = 0; x < _numBins+1; x++ ) {
                float jval = _jhist[ y * ( _numBins + 1 ) + x ];
                mi += jval * Math::log( ( jval + 1e-10f ) / ( _thist[ y ] * _whist[ x ] + 1e-10f ) );
            }
        }

        return mi;
    }

    inline void MITracker::updateInputHistograms()
    {
        size_t stride;
        size_t tstride;
        const float* ptr  = _warped.map<float>( &stride );
        const float* tptr = _itemplate.map<float>( &tstride );
        size_t w, h;
        float sum = 0;

        for( size_t i = 0; i < ( _numBins + 1 ) * ( _numBins + 1 ); i++ )
            _jhist[ i ] = 0;

        w = _itemplate.width();
        h = _itemplate.height();

        const float* pi = ptr;
        const float* pit = tptr;
        const float norm = w * h;

        float splm;
        while( h-- ) {
            size_t n = w;
            const float* pval = pi;
            const float* ptval = pit;
            while( n-- ) {
                float r, t;
                t = *pval++ * ( float ) ( _numBins - 3 ) + 1.0f;
                r = *ptval++ * ( float ) ( _numBins - 3 ) + 1.0f;
                int tidx = ( int ) t;
                int ridx = ( int ) r;
                for( int m = -1; m <= 2; m++ ) {
                    splm = BSplinef::eval( -r + ( float ) ( ridx + m ) );
                    _jhist[ ( ridx + m ) *  ( _numBins + 1 ) + ( tidx - 1 ) ] += BSplinef::eval( -t + ( float ) ( tidx - 1 ) ) * splm;
                    _jhist[ ( ridx + m ) *  ( _numBins + 1 ) + ( tidx + 0 ) ] += BSplinef::eval( -t + ( float ) ( tidx + 0 ) ) * splm;
                    _jhist[ ( ridx + m ) *  ( _numBins + 1 ) + ( tidx + 1 ) ] += BSplinef::eval( -t + ( float ) ( tidx + 1 ) ) * splm;
                    _jhist[ ( ridx + m ) *  ( _numBins + 1 ) + ( tidx + 2 ) ] += BSplinef::eval( -t + ( float ) ( tidx + 2 ) ) * splm;
                }
            }
            pi += stride;
            pit += tstride;
        }

        _warped.unmap( ptr );
        _itemplate.unmap( tptr );

        sum = 1.0f / norm;
        for( size_t i = 0; i < ( _numBins + 1 ) * ( _numBins + 1 ); i++ )
            _jhist[ i ] *= sum;
    }

    inline void MITracker::updateTemplateHistogram()
    {
        size_t tstride;
        const float* tptr = _itemplate.map<float>( &tstride );
        size_t w, h;
        float sum = 0;

        for( size_t i = 0; i < ( _numBins + 1 ); i++ )
            _thist[ i ] = 0;

        w = _itemplate.width();
        h = _itemplate.height();

        const float* pit = tptr;
        const float norm = w * h;

        while( h-- ) {
            size_t n = w;
            const float* ptval = pit;
            while( n-- ) {
                float r;
                r = *ptval++ * ( float ) ( _numBins - 3 ) + 1.0f;
                int ridx = ( int ) r;
                for( int m = -1; m <= 2; m++ ) {
                    float splm = BSplinef::eval( -r + ( float ) ( ridx + m ) );
                    _thist[ ( ridx + m ) ] += splm;
                }
            }
            pit += tstride;
        }
        _itemplate.unmap( tptr );

        sum = 1.0f / norm;
        for( size_t i = 0; i < ( _numBins + 1 ); i++ )
            _thist[ i ] *= sum;
    }

    inline void MITracker::updateApproxHessian()
    {
        _warped = _itemplate;
        updateInputHistograms();

        size_t binStride = _numBins + 1;
        size_t binsSqr = Math::sqr( binStride );
        JacType* binJac = new JacType[ binsSqr ];

        for( size_t b = 0; b < binsSqr; b++ ) {
                binJac[ b ].setZero();
        }

        float _c1[ binsSqr ];
        float _c2[ binsSqr ];
        float* c1 = _c1;
        float* c2 = _c2;
        for( size_t y = 0; y < binStride; y++ ) {
            for( size_t x = 0; x < binStride; x++ ) {
                float jval = _jhist[ y * binStride + x ] + 1e-10f;
                float tval = _thist[ y ] + 1e-10f;
                *c1++ = Math::log( jval / tval );
                *c2++ = ( 1.0f / jval - 1.0f / tval );
            }
        }

        IMapScoped<const float> map( _warped );

        size_t w, h;
        w = _itemplate.width();
        h = _itemplate.height();
        const float norm = w*h;

        std::vector<int>::const_iterator rBin = _binValues.begin();

        HessType hessSum = HessType::Zero();
        float sumJtJ, sumH;

        for( size_t y = 0; y < h; y++ ) {
            const float* pval = map.ptr();
            for( size_t x = 0; x < w; x++, rBin++ ) {
                if( !_evaluated[ y * w + x ] ){
                    pval++;
                    continue;
                }

                const JacType& currJac = _jTemp[ y * w + x ];

                // get the bin value for the current pixel of the warped image
                float t;
                t = *pval++ * (float)( _numBins - 3 ) + 1.0f;
                int tidx = ( int )t;

                // sum of coeffs for the image hessian
                sumH = 0.0f;
                sumJtJ = 0.0f;
                for( int m = -1; m <= 2; m++ ) {
                    float spl = BSplinef::eval( -t + ( float ) ( tidx + m ) );
                    for( int o = -1; o <= 2; o++ ) {
                        const float sigma = spl * _c1[ ( *rBin + o ) * binStride + tidx + m ];
                        float tmp = _tempSplineWeights[ ( y * w + x ) * 4 + ( o + 1 ) ] * sigma;
                        sumH   += tmp;
                        sumJtJ += _tempSplineDerivWeights[ ( y * w + x ) * 4 + ( o + 1 ) ] * sigma;
                        binJac[ ( *rBin + o ) * binStride + tidx + m ] += tmp * currJac;
                    }
                }
                hessSum -= _hTemp[ y * w + x ] * sumH;
                hessSum += currJac * currJac.transpose() * sumJtJ;
            }
            map++;
        }

        hessSum /= (float)norm;


        const float normInvSqr = 1.0f / Math::sqr( norm );
        c2 = _c2;
        const JacType* J = binJac;
        for( size_t b = 0; b < binsSqr; b++, c2++, J++ ) {
            hessSum += normInvSqr * *c2 * *J * J->transpose();
        }
        _miHessian = hessSum.inverse();
        std::cout << "inverse Hessian: " << _miHessian << std::endl;

        delete[] binJac;
    }

    inline void MITracker::updateDerivatives()
    {
        float c1[ ( _numBins + 1 ) ][ ( _numBins + 1 ) ];
        for( size_t y = 0; y < _numBins+1; y++ ) {
            for( size_t x = 0; x < _numBins+1; x++ ) {
                float jval = _jhist[ y * ( _numBins + 1 ) + x ] + 1e-10f;
                float tval = _thist[ y ] + 1e-10f;
                //c1[ y ][ x ] = Math::log( jval / tval );
                c1[ y ][ x ] = Math::fastLog( jval / tval );
            }
        }

        size_t stride;
        const float* ptr = _warped.map<float>( &stride );
        size_t w, h;

        w = _itemplate.width();
        h = _itemplate.height();

        const float* pi = ptr;
        const float norm = w*h;

        std::vector<int>::const_iterator rBin = _binValues.begin();

        _miJacobian.setZero();

        float sumJ, sumTmp;
        for( size_t y = 0; y < h; y++ ) {
            const float* pval = pi;
            for( size_t x = 0; x < w; x++, rBin++ ) {
                if( !_evaluated[ y * w + x ] ){
                    pval++;
                    continue;
                }

                float t;
                t = *pval++ * (float)( _numBins - 3 ) + 1.0f;
                int tidx = ( int )t;

                sumJ = 0.0f;
                for( int m = -1; m <= 2; m++ ) {
                    sumTmp = 0.0f;
                    float spl = BSplinef::eval( -t + ( float ) ( tidx + m ) );
                    sumTmp += _tempSplineWeights[ ( y * w + x ) * 4 + 0 ] * c1[ *rBin - 1 ][ tidx + m ];
                    sumTmp += _tempSplineWeights[ ( y * w + x ) * 4 + 1 ] * c1[ *rBin + 0 ][ tidx + m ];
                    sumTmp += _tempSplineWeights[ ( y * w + x ) * 4 + 2 ] * c1[ *rBin + 1 ][ tidx + m ];
                    sumTmp += _tempSplineWeights[ ( y * w + x ) * 4 + 3 ] * c1[ *rBin + 2 ][ tidx + m ];
                    sumJ += spl * sumTmp;
                }
                _miJacobian -= _jTemp[ y * w + x ] * sumJ;
            }
            pi += stride;
        }

        _miJacobian /= norm;
        _warped.unmap( ptr );

    }

    inline void MITracker::offlineTemplateDerivatives()
    {
        if( _jTemp )
            delete[] _jTemp;        
        if( _hTemp )
            delete[] _hTemp;
        if( _tempSplineWeights )
            delete[] _tempSplineWeights;
        if( _tempSplineDerivWeights )
            delete[] _tempSplineDerivWeights;
        if( _evaluated )
            delete[] _evaluated;

        size_t numPixel = _itemplate.width() * _itemplate.height();

        // we need numbins times pixel values
        _jTemp = new Eigen::Matrix<float, NUMPARAMS, 1>[ numPixel ];        
        _hTemp = new Eigen::Matrix<float, NUMPARAMS, NUMPARAMS>[ numPixel ];
        _tempSplineWeights = new float[ 4 * numPixel ];
        _tempSplineDerivWeights= new float[ 4 * numPixel ];
        _evaluated = new bool[ numPixel ];

        _binValues.clear();
        _binValues.reserve( numPixel );

        Eigen::Matrix<float, 2, NUMPARAMS> screenJac;
        //Vector2f p;
        Eigen::Vector2f p;

        size_t iStride;
        size_t gxStride, gyStride;
        size_t gxyStride, gxxStride, gyyStride;
        const float * i_ptr = _itemplate.map<float>( &iStride );
        const float * g_x = _templateGradX.map<float>( &gxStride );
        const float * g_y = _templateGradY.map<float>( &gyStride );
        const float * g_xx = _templateGradXX.map<float>( &gxxStride );
        const float * g_yy = _templateGradYY.map<float>( &gyyStride );
        const float * g_xy = _templateGradXY.map<float>( &gxyStride );
        const float * gx = g_x;
        const float * gy = g_y;
        const float * gxx = g_xx;
        const float * gyy = g_yy;
        const float * gxy = g_xy;
        const float* iptr = i_ptr;

        Eigen::Matrix<float, 1, 2> grad;
        Eigen::Matrix<float, 2, 2> hess;
        Eigen::Matrix<float, 1, NUMPARAMS> imagePoseDeriv;
        Eigen::Matrix<float, NUMPARAMS, NUMPARAMS> imagePoseDeriv2, wx, wy;

        float splineDeriv;
        float splineDeriv2;

        size_t iter = 0;
        float normFactor = ( float ) ( _numBins - 3 );
        float pixVal;

        for( size_t y = 0; y < _itemplate.height(); y++ ){
            p[ 1 ] = y;
            for( size_t x = 0; x < _itemplate.width(); x++, iter++ ){
                // first order image derivatives
                grad[ 0 ] = gx[ x ];
                grad[ 1 ] = gy[ x ];

                if( grad.norm() < _gradThresh ){
                    // do not use this pixel
                    _evaluated[ iter ] = false;
                } else {
                    _evaluated[ iter ] = true;
                }

                // second order image derivatives
                //hess << gxx[ x ], gxy[ x ], gxy[ x ], gyy[ x ];
                hess( 0, 0 ) = gxx[ x ];
                hess( 0, 1 ) = gxy[ x ];
                hess( 1, 0 ) = gxy[ x ];
                hess( 1, 1 ) = gyy[ x ];

                grad *= ( float )( _numBins - 3 );
                hess *= ( float )( _numBins - 3 );

                p[ 0 ] = x;
                _pose.screenJacobian( screenJac, p );
                _pose.screenHessian( wx, wy, p );

                _jTemp[ iter ] = (grad * screenJac).transpose();                
                _hTemp[ iter ] = screenJac.transpose() * hess * screenJac + grad[ 0 ] * wx + grad[ 1 ] * wy;

                pixVal = normFactor * iptr[ x ] + 1.0f;
                int binIdx = ( int ) pixVal;
                _binValues.push_back( binIdx );
                for( int bin = 0; bin < 4; bin++ ){
                    splineDeriv = BSpline<float>::evalDerivative( (float)( binIdx + bin - 1 ) - pixVal );
                    splineDeriv2 = BSpline<float>::evalSecondDerivative( (float)( binIdx + bin - 1 ) - pixVal );
                    _tempSplineWeights[ 4 * iter + bin ] = splineDeriv;
                    _tempSplineDerivWeights[ 4 * iter + bin ] = splineDeriv2;
                }
            }
            gx   += gxStride;
            gy   += gyStride;
            gxx  += gxxStride;
            gyy  += gyyStride;
            gxy  += gxyStride;
            iptr += iStride;
        }
        _itemplate.unmap( i_ptr );
        _templateGradX.unmap( g_x );
        _templateGradY.unmap( g_y );
        _templateGradXX.unmap( g_xx );
        _templateGradYY.unmap( g_yy );
        _templateGradXY.unmap( g_xy );
    }

    inline void MITracker::setPose( const Matrix3f & hom )
    {
        //_pose.pose() = hom;
        Eigen::Matrix<float, 3, 3> & h = _pose.transformation();
        h( 0, 0 ) = hom[ 0 ][ 0 ];
        h( 0, 1 ) = hom[ 0 ][ 1 ];
        h( 0, 2 ) = hom[ 0 ][ 2 ];
        h( 1, 0 ) = hom[ 1 ][ 0 ];
        h( 1, 1 ) = hom[ 1 ][ 1 ];
        h( 1, 2 ) = hom[ 1 ][ 2 ];
        h( 2, 0 ) = hom[ 2 ][ 0 ];
        h( 2, 1 ) = hom[ 2 ][ 1 ];
        h( 2, 2 ) = hom[ 2 ][ 2 ];
    }
}

#endif
