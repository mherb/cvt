#include <cvt/gfx/Image.h>
#include <cvt/gfx/IKernel.h>
#include <cvt/gfx/ifilter/ITransform.h>
#include <cvt/math/BSpline.h>
#include <cvt/gfx/IHistogram.h>

#include "PoseHomography.h"

namespace cvt {
	class MITracker {
		public:
			MITracker();
			~MITracker();

			void	updateInput( const Image& img );

			/**
			 *	@param	img	the current image (grayscale)
			 *	@param	pos	the position of the template in the current image
			 */
			void	updateTemplate( const Image& img, const Vector2f& pos );

			const Matrix3f& pose() const;

			const Image&	templateImage() const { return _itemplate; }
			const Image&	templateGradX() const { return _templateGradX; }
			const Image&	templateGradY() const { return _templateGradY; }
			const Image&	warped()		const { return _warped; }

			size_t	maxIterations()			const { return _maxIter; }
			void	setMaxIterations( size_t m )  { _maxIter = m; }

		private:
			void	updateInputHistograms();
			void	updateDerivatives();

			void	updateTemplateGradients();
			void	offlineTemplateDerivatives();

			void	solveDeltaPose();

			// Histogram related
			size_t	_numBins;

			// Derivatives / Histograms
			Image	_itemplate;
			Image	_templateGradX;
			Image	_templateGradY;
			Image	_templateGradXX;
			Image	_templateGradYY;
			Image	_templateGradXY;
			IHistogramf _templateHist;

			// Joint histogram
			float*	_jhist;

			// backprojection of image to template space
			Image	_warped;
			PoseHomography<float> _pose;

			Eigen::Matrix<float, 8, 1>	_miJacobian;
			Eigen::Matrix<float, 8, 8>	_miHessian;


			// for the template we can calculate offline data once:
			Eigen::Matrix<float, 8, 1>*  _jTemp;
			Eigen::Matrix<float, 8, 8>*  _hTemp;

			// optimization related:
			size_t	_maxIter;

	};

	inline MITracker::MITracker() :
		_numBins( 10 ),
		_templateHist( _numBins ),
		_jTemp( 0 ),
		_hTemp( 0 ),
		_maxIter( 1 )
	{
		_jhist = new float[ ( _numBins + 1 ) * ( _numBins + 1 ) ];
	}

	inline MITracker::~MITracker()
	{
		delete[] _jhist;
		if( _jTemp )
			delete[] _jTemp;
		if( _hTemp )
			delete[] _hTemp;
	}

	inline void MITracker::updateInput( const Image & _img )
	{
		size_t iter = 0;
		Matrix3f hinv;
		Image img;
		_img.convert( img, IFormat::GRAY_FLOAT );

		while( iter < _maxIter ){
			// warp back according to current pose
			const Matrix3f & h = _pose.pose();
			hinv = h.inverse();
			ITransform::apply( _warped, img, hinv );

			// calculate the online stuff:
			updateInputHistograms();
			updateDerivatives();

			std::cout << "Jacobian:\n" << _miJacobian << std::endl;
			std::cout << "Hessian:\n" << _miHessian << std::endl;

			//_miHessian = - _miJacobian * _miJacobian.transpose();
			//std::cout << "Hessian:\n" << _miHessian << std::endl;

			solveDeltaPose();	

			// TODO: check MI for early step out?
			iter++;
		}

	}

	inline void MITracker::solveDeltaPose()
	{
		// calc the update:
		Eigen::Matrix<float, 8, 1> delta;
		delta = -_miHessian.inverse() * _miJacobian;
		_pose.addDelta( delta );
	}

	inline void MITracker::updateTemplateGradients()
	{
		Image tmpf;
		_itemplate.convert( tmpf, IFormat::GRAY_FLOAT );

		_templateGradX.reallocate( _itemplate.width(), _itemplate.height(), tmpf.format() );
		_templateGradY.reallocate( _itemplate.width(), _itemplate.height(), tmpf.format() );
		_templateGradXX.reallocate( _itemplate.width(), _itemplate.height(), tmpf.format() );
		_templateGradYY.reallocate( _itemplate.width(), _itemplate.height(), tmpf.format() );
		_templateGradXY.reallocate( _itemplate.width(), _itemplate.height(), tmpf.format() );

		tmpf.convolve( _templateGradX, IKernel::HAAR_HORIZONTAL_3 );
		tmpf.convolve( _templateGradY, IKernel::HAAR_VERTICAL_3 );
		tmpf.convolve( _templateGradXX, IKernel::LAPLACE_XX );
		tmpf.convolve( _templateGradYY, IKernel::LAPLACE_YY );
		tmpf.convolve( _templateGradXY, IKernel::HAAR_HORIZONTAL_3, IKernel::HAAR_VERTICAL_3 );
	}

	inline void MITracker::updateTemplate( const Image& img, const Vector2f& pos )
	{
		Time t;
		_pose.pose().setHomography( 0.0f, 0.0f, 1.0f, 1.0f, pos.x, pos.y, 0.0f, 0.0f );
		img.convert( _itemplate, IFormat::GRAY_FLOAT );
		updateTemplateGradients();
		offlineTemplateDerivatives();

		_templateHist.update( _itemplate );

		// reallocate the backwarped sizes
		_warped.reallocate( _itemplate );

		std::cout << "Template update took: " << t.elapsedMilliSeconds() << std::endl;
	}

	inline const Matrix3f& MITracker::pose() const
	{
		return _pose.pose();
	}

	inline void MITracker::updateInputHistograms()
	{
		size_t stride;
		size_t tstride;
		const float* ptr = _warped.map<float>( &stride );
		const float* tptr = _itemplate.map<float>( &tstride );
		size_t w, h;
		float sum = 0;

		for( size_t i = 0; i < ( _numBins + 1 ) * ( _numBins + 1 ); i++ )
			_jhist[ i ] = 0;

		w = _itemplate.width();
		h = _itemplate.height();

		const float* pi = ptr;
		const float* pit = tptr;

		while( h-- ) {
			size_t n = w;
			const float* pval = pi;
			const float* ptval = pit;
			while( n-- ) {
				float r, t;
				t = *pval * ( _numBins - 3 ) + 1.0f;
				r = *ptval * ( _numBins - 3 ) + 1.0f;
				int tidx = ( int ) t;
				int ridx = ( int ) r;
				for( int m = -1; m <= 2; m++ ) {
					for( int o = -1; o <= 2; o++ ) {
						float val = BSplinef::eval( -t + ( float ) ( tidx + o ) )
							* BSplinef::eval( -r + ( float ) ( ridx + m ) );
						_jhist[ ( ridx + m ) *  ( _numBins + 1 ) + ( tidx + o ) ] += val;
						sum += val;
					}
				}
			}
			pi += stride;
			pit += tstride;
		}

		_warped.unmap( ptr );
		_itemplate.unmap( tptr );

		sum = 1.0f / sum;
		for( size_t i = 0; i < ( _numBins + 1 ) * ( _numBins + 1 ); i++ )
			_jhist[ i ] *= sum;
	}

	inline void MITracker::updateDerivatives()
	{
		_miJacobian.setZero();
		_miHessian.setZero();

		size_t stride;
		size_t tstride;
		const float* ptr = _warped.map<float>( &stride );
		const float* tptr = _itemplate.map<float>( &tstride );
		size_t w, h;

		w = _itemplate.width();
		h = _itemplate.height();

		const float* pi = ptr;
		const float* pit = tptr;

		for( size_t y = 0; y < h; y++ ) {
			const float* pval = pi;
			const float* ptval = pit;
			for( size_t x = 0; x < w; x++ ) {
				float r, t;
				t = *pval * ( _numBins - 3 ) + 1.0f;
				r = *ptval * ( _numBins - 3 ) + 1.0f;
				int tidx = ( int ) t;
				int ridx = ( int ) r;
				for( int m = -1; m <= 2; m++ ) {
					for( int o = -1; o <= 2; o++ ) {
						float jh = _jhist[ ( ridx + m ) *  ( _numBins + 1 ) + ( tidx + o ) ] + 1e-5f;
						float ht = _templateHist( ridx + o ) + 1e-5f;
						float c = 1.0f + Math::log( jh / ht );
						c *= BSplinef::eval( -t + ( float ) ( tidx + o ) );
						_miJacobian += c * _jTemp[ (  y * w + x ) * _numBins + ( ridx + m ) ];
						_miHessian += c * _hTemp[ (  y * w + x ) * _numBins + ( ridx + m ) ];
						c = 1.0f / jh - 1.0f / ht;
						_miHessian += c * _jTemp[ (  y * w + x ) * _numBins + ( ridx + m ) ] * _jTemp[ (  y * w + x ) * _numBins + ( ridx + m ) ].transpose();
					}
				}
			}
			pi += stride;
			pit += tstride;
		}
		float norm = w * h;
		_miJacobian /= norm;
		_miHessian /= norm;

		_warped.unmap( ptr );
		_itemplate.unmap( tptr );
	}

	inline void MITracker::offlineTemplateDerivatives()
	{
		if( _jTemp )
			delete[] _jTemp;
		if( _hTemp )
			delete[] _hTemp;
		
		size_t numPixel = _itemplate.width() * _itemplate.height();

		// we need numbins times pixel values
		_jTemp = new Eigen::Matrix<float, 8, 1>[ _numBins * numPixel ];
		_hTemp = new Eigen::Matrix<float, 8, 8>[ _numBins * numPixel ];

		Eigen::Matrix<float, 2, 8> screenJac;
		Vector2f p;

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
		const float * gxx = gxx;
		const float * gyy = gyy;
		const float * gxy = gxy;
		const float* iptr = i_ptr;

		Eigen::Matrix<float, 1, 2> grad;
		Eigen::Matrix<float, 2, 2> hess;
		Eigen::Matrix<float, 1, 8> imagePoseDeriv;
		Eigen::Matrix<float, 8, 8> imagePoseDeriv2, wx, wy;

		float splineDeriv = 1.0f;
		float splineDeriv2 = 1.0f;

		size_t iter = 0;
		float normFactor = ( float ) ( _numBins - 3 );
		float pixVal;
		for( size_t y = 0; y < _itemplate.height(); y++ ){
			p.y = y;
			for( size_t x = 0; x < _itemplate.width(); x++, iter++ ){
				// first order image derivatives
				grad[ 0 ] = -gx[ x ];
				grad[ 1 ] = -gy[ x ];

				// second order image derivatives
				hess( 0, 0 ) = gxx[ x ];
				hess( 0, 1 ) = hess( 1, 0) = -gxy[ x ];
				hess( 1, 1 ) = gyy[ x ];

				p.x = x;
				_pose.screenJacobian( screenJac, p );
				_pose.screenHessian( wx, wy, p );

				imagePoseDeriv = grad * screenJac;
				
				// second order image pose derivative
				imagePoseDeriv2 = screenJac.transpose() * hess * screenJac + grad[ 0 ] * wx + grad[ 1 ] * wy; 
			
				pixVal = normFactor * iptr[ x ] + 1.0f;
				for( size_t bin = 0; bin < _numBins; bin++ ){
					// first order mi part: TODO: evaluate first and second order splineDerivatives!
					splineDeriv = BSpline<float>::evalDerivative( (float)bin - pixVal ); 
					splineDeriv2 = BSpline<float>::evalSecondDerivative( (float)bin - pixVal );
					_jTemp[ _numBins * iter + bin ] = -( splineDeriv * imagePoseDeriv ).transpose();
					_hTemp[ _numBins * iter + bin ] = splineDeriv2 * imagePoseDeriv.transpose() * imagePoseDeriv - splineDeriv * imagePoseDeriv2;
				}
			}
			gx += gxStride;
			gy += gyStride;
			gxx += gxxStride;
			gyy += gyyStride;
			gxy += gxyStride;
			iptr += iStride;
		}
		_itemplate.unmap( i_ptr );
		_templateGradX.unmap( g_x );
		_templateGradY.unmap( g_y );
		_templateGradXX.unmap( g_xx );
		_templateGradYY.unmap( g_yy );
		_templateGradXY.unmap( g_xy );
	}

}
