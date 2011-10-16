#include <cvt/gfx/Image.h>
#include <cvt/gfx/IKernel.h>
#include <cvt/gfx/ifilter/ITransform.h>
#include <cvt/math/BSpline.h>
#include <cvt/gfx/IHistogram.h>

#include "PoseHomography.h"
#include "PoseTranslation.h"

#include <cvt/math/SL3.h>

#define NUMPARAMS 8

namespace cvt {
	class MITracker {
		public:
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW

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

		private:
			void	updateInputHistograms();
			void	updateDerivatives();

			void	updateTemplateGradients();
			void	offlineTemplateDerivatives();

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
			IHistogramf _templateHist;

			// Joint histogram
			float*	_jhist;

			// backprojection of image to template space
			Image	_warped;
			
			//PoseHomography<float> _pose;
			//PoseTranslation<float> _pose;
			SL3<float>	_pose;

			Eigen::Matrix<float, NUMPARAMS, 1>	_miJacobian;
			Eigen::Matrix<float, NUMPARAMS, NUMPARAMS>	_miHessian;


			// for the template we can calculate offline data once:
			Eigen::Matrix<float, NUMPARAMS, 1>*  _jTemp;
			Eigen::Matrix<float, NUMPARAMS, NUMPARAMS>*  _jTempOuter;
			Eigen::Matrix<float, NUMPARAMS, NUMPARAMS>*  _hTemp;
			std::vector<size_t>	 _binValues;

			// optimization related:
			size_t	_maxIter;

			Matrix3f	_currPose;

	};

	inline MITracker::MITracker() :
		_numBins( 64 ),
		_templateHist( _numBins ),
		_jTemp( 0 ),
		_jTempOuter( 0 ),
		_hTemp( 0 ),
		_maxIter( 10 )
	{
		_jhist = new float[ ( _numBins + 1 ) * ( _numBins + 1 ) ];
	}

	inline MITracker::~MITracker()
	{
		delete[] _jhist;
		if( _jTemp )
			delete[] _jTemp;
		if( _jTempOuter )
			delete[] _jTempOuter;
		if( _hTemp )
			delete[] _hTemp;
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
			//const Matrix3f & h = _pose.pose();
			const Matrix3f & h = pose();
			hinv = h.inverse();

			tmp.fill( Color::WHITE );
			ITransform::apply( tmp, img, hinv );
			tmp.convolve( _warped, IKernel::GAUSS_HORIZONTAL_3, IKernel::GAUSS_VERTICAL_3 );

			// calculate the online stuff:
			updateInputHistograms();
			updateDerivatives();

			//std::cout << "Jacobian:\n" << _miJacobian << std::endl;
			//std::cout << "Hessian:\n" << _miHessian << std::endl;

			float epsilon = solveDeltaPose();

			if( epsilon < 1e-5 )
				return;	

			iter++;
		}

	}

	inline float MITracker::solveDeltaPose()
	{
		// calc the update:
		Eigen::Matrix<float, NUMPARAMS, 1> delta;
		delta = -_miHessian.inverse() * _miJacobian;
		//std::cout << "Delta:\n" << delta << std::endl;
		_pose.applyInverse( delta );

		return delta.norm();
	}

	inline void MITracker::updateTemplateGradients()
	{
		_templateGradX.reallocate( _itemplate.width(), _itemplate.height(), _itemplate.format() );
		_templateGradY.reallocate( _itemplate.width(), _itemplate.height(), _itemplate.format() );
		_templateGradXX.reallocate( _itemplate.width(), _itemplate.height(), _itemplate.format() );
		_templateGradYY.reallocate( _itemplate.width(), _itemplate.height(), _itemplate.format() );
		_templateGradXY.reallocate( _itemplate.width(), _itemplate.height(), _itemplate.format() );

		_itemplate.convolve( _templateGradX, IKernel::HAAR_HORIZONTAL_3 );
		_itemplate.convolve( _templateGradY, IKernel::HAAR_VERTICAL_3 );
		_itemplate.convolve( _templateGradXX, IKernel::LAPLACE_3_XX );
		_itemplate.convolve( _templateGradYY, IKernel::LAPLACE_3_YY );


		//_templateGradX.convolve( _templateGradXX, IKernel::HAAR_HORIZONTAL_3 );
		//_templateGradY.convolve( _templateGradYY, IKernel::HAAR_VERTICAL_3 );
		IKernel kxy1( IKernel::HAAR_HORIZONTAL_3 );
		kxy1.scale( 0.5f );
		IKernel kxy2( IKernel::HAAR_VERTICAL_3 );
		kxy2.scale( 0.5f );
		_itemplate.convolve( _templateGradXY, kxy1, kxy2 );

		//_templateGradXX.save("grad_XX.png");
		//_templateGradYY.save("grad_YY.png");
		//_templateGradXY.save("grad_XY.png");
	}

	inline void MITracker::updateTemplate( const Image& img, const Vector2f& pos )
	{
		Time t;
		//_pose.pose().setHomography( 0.0f, 0.0f, 1.0f, 1.0f, pos.x, pos.y, 0.0f, 0.0f );
		_pose.set( 0.0f, 0.0f, 1.0f, 1.0f, pos.x, pos.y, 0.0f, 0.0f );
		Image tmp;
		img.convert( tmp, IFormat::GRAY_FLOAT );
		_itemplate.reallocate( tmp );
		tmp.convolve( _itemplate, IKernel::GAUSS_HORIZONTAL_3, IKernel::GAUSS_VERTICAL_3 );
		updateTemplateGradients();
		offlineTemplateDerivatives();

		_templateHist.update( _itemplate );

		// reallocate the backwarped sizes
		_warped.reallocate( _itemplate );

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
				t = *pval++ * ( float ) ( _numBins - 3 ) + 1.0f;
				r = *ptval++ * ( float ) ( _numBins - 3 ) + 1.0f;
				int tidx = ( int ) t;
				int ridx = ( int ) r;
				for( int m = -1; m <= 2; m++ ) {
					float splm = BSplinef::eval( -r + ( float ) ( ridx + m ) );
					for( int o = -1; o <= 2; o++ ) {
						float val = BSplinef::eval( -t + ( float ) ( tidx + o ) ) * splm;
						_jhist[ ( ridx + m ) *  ( _numBins + 1 ) + ( tidx + o ) ] += val;
					}
				}
			}
			pi += stride;
			pit += tstride;
		}

		_warped.unmap( ptr );
		_itemplate.unmap( tptr );

		sum = 1.0f / ( _itemplate.width() * _itemplate.height() );
		for( size_t i = 0; i < ( _numBins + 1 ) * ( _numBins + 1 ); i++ )
			_jhist[ i ] *= sum;
	}

	inline void MITracker::updateDerivatives()
	{
	//	Eigen::Matrix<float,1,NUMPARAMS> curJac;
	//	Eigen::Matrix<float,NUMPARAMS,NUMPARAMS> curHess;
		_miJacobian.setZero();
		_miHessian.setZero();


		Eigen::Matrix<float,1,NUMPARAMS> allJac[ ( _numBins + 1 ) ][ ( _numBins + 1 ) ];
		Eigen::Matrix<float,NUMPARAMS,NUMPARAMS> allHess[ ( _numBins + 1 ) ][ ( _numBins + 1 ) ];

		for( size_t y = 0; y < _numBins; y++ ) {
			for( size_t x = 0; x < _numBins; x++ ) {
				allJac[ y ][ x ].setZero();
				allHess[ y ][ x ].setZero();
			}
		}

		size_t stride;
		const float* ptr = _warped.map<float>( &stride );
		size_t w, h;

		w = _itemplate.width();
		h = _itemplate.height();

		const float* pi = ptr;
		const float norm = w * h;

		std::vector<size_t>::const_iterator rBin = _binValues.begin();

		for( size_t y = 0; y < h; y++ ) {
			const float* pval = pi;
			for( size_t x = 0; x < w; x++, rBin++ ) {
				float t;
				t = *pval++ * ( _numBins - 3 ) + 1.0f;
				int tidx = ( int ) t;
				//curJac.setZero();
				//curHess.setZero();

				for( int o = -1; o <= 2; o++ ) {
					float spl= BSplinef::eval( -t + ( float ) ( tidx + o ) );
				//	float ht = _templateHist( *rBin + o ) + 1e-6f;
					for( int m = -1; m <= 2; m++ ) {
				//		float jh = _jhist[ ( *rBin + m ) *  ( _numBins + 1 ) + ( tidx + o ) ] + 1e-6f;
				//		float c = 1.0f + Math::fastLog( jh / ht );
						/*float d = Math::abs( c - ( 1.0f + Math::log( jh / ht ) ) );
						if( d > 1e-4f )
						std::cout << d << std::endl;*/
				//		c *= spl;
						allJac[ *rBin + m ][ tidx + o ] += spl  * _jTemp[ (  y * w + x ) * 4 + ( m + 1 ) ];
						allHess[ *rBin + m ][ tidx + o ] += spl * _hTemp[ (  y * w + x ) * 4 + ( m + 1 ) ];
				//		curJac += c * _jTemp[ (  y * w + x ) * 4 + ( m + 1 ) ] / norm;
				//		curHess += c * _hTemp[ (  y * w + x ) * 4 + ( m + 1 ) ] / ( norm );
				//		c = 1.0f / jh - 1.0f / ht;
				//		c *= spl * spl;
				//		curHess += c * _jTempOuter[ (  y * w + x ) * 4 + ( m + 1 ) ] / Math::sqr( norm );
					}
				}
				//_miJacobian += curJac;
				//_miHessian += curHess;
			}
			pi += stride;
		}

		_warped.unmap( ptr );

		for( size_t y = 0; y < _numBins; y++ ) {
			float ht = _templateHist( y ) + 1e-6f;
			for( size_t x = 0; x < _numBins; x++ ) {
				float jh = _jhist[ y *  ( _numBins + 1 ) + x ] + 1e-6f;
				float c = 1.0f + Math::fastLog( jh / ht );
				_miJacobian += c * allJac[ y ][ x ] / norm;
				_miHessian += c * allHess[ y ][ x ] / norm;
				c = 1.0f / jh - 1.0f / ht;
				_miHessian += c * allJac[ y ][ x ].transpose() * allJac[ y ][ x ] / Math::sqr( norm );
			}
		}
	}

	inline void MITracker::offlineTemplateDerivatives()
	{
		if( _jTemp )
			delete[] _jTemp;
		if( _jTempOuter )
			delete[] _jTempOuter;
		if( _hTemp )
			delete[] _hTemp;
		
		size_t numPixel = _itemplate.width() * _itemplate.height();

		// we need numbins times pixel values
		_jTemp = new Eigen::Matrix<float, NUMPARAMS, 1>[ 4 * numPixel ];
		_jTempOuter = new Eigen::Matrix<float, NUMPARAMS, NUMPARAMS>[ 4 * numPixel ];
		_hTemp = new Eigen::Matrix<float, NUMPARAMS, NUMPARAMS>[ 4 * numPixel ];
		_binValues.clear();
		_binValues.reserve( numPixel );

		Eigen::Matrix<float, 2, NUMPARAMS> screenJac;
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

				// second order image derivatives
				//hess << gxx[ x ], gxy[ x ], gxy[ x ], gyy[ x ];
				hess( 0, 0 ) = gxx[ x ];
				hess( 0, 1 ) = gxy[ x ];
				hess( 1, 0 ) = gxy[ x ];
				hess( 1, 1 ) = gyy[ x ];
				
				grad *= ( float )(_numBins - 3);
				hess *= ( float )(_numBins - 3);

				p[ 0 ] = x;
				_pose.screenJacobian( screenJac, p );

				_pose.screenHessian( wx, wy, p );

				imagePoseDeriv = grad * screenJac;
				
				// second order image pose derivative
				//std::cout << screenJac.transpose() * screenJac << std::endl << std::endl;
				imagePoseDeriv2 = screenJac.transpose() * hess * screenJac + grad[ 0 ] * wx + grad[ 1 ] * wy; 
			
				pixVal = normFactor * iptr[ x ] + 1.0f;
				size_t binIdx = (size_t)pixVal;
				_binValues.push_back( binIdx );
				for( int bin = 0; bin < 4; bin++ ){
					splineDeriv = BSpline<float>::evalDerivative( (float)( binIdx + bin - 1 ) - pixVal ); 
					splineDeriv2 = BSpline<float>::evalSecondDerivative( (float)( binIdx + bin - 1 ) - pixVal );
					_jTemp[  4 * iter + bin ] = - splineDeriv * imagePoseDeriv.transpose();
					_jTempOuter[ 4 * iter + bin ] = _jTemp[ 4 * iter + bin ] * _jTemp[ 4 * iter + bin ].transpose();
					_hTemp[ 4 * iter + bin ] = splineDeriv2 * imagePoseDeriv.transpose() * imagePoseDeriv - splineDeriv * imagePoseDeriv2;
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

	inline void MITracker::setPose( const Matrix3f & hom )
	{
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
