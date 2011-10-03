#include <cvt/gfx/Image.h>
#include <cvt/gfx/IKernel.h>
#include <cvt/gfx/ifilter/ITransform.h>
#include <cvt/math/BSpline.h>

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
			void	updateTemplateHistograms();
			void	offlineTemplateDerivatives();

			void	solveDeltaPose();

			Image	_itemplate;
			Image	_templateGradX;
			Image	_templateGradY;
			Image	_templateGradXX;
			Image	_templateGradYY;
			Image	_templateGradXY;

			// backprojection of image to template space
			Image	_warped;
			PoseHomography<float> _pose;

			Eigen::Matrix<float, 8, 1>	_miJacobian;
			Eigen::Matrix<float, 8, 8>	_miHessian;

			// Histogram related
			size_t	_numBins;

			// for the template we can calculate offline data once:
			Eigen::Matrix<float, 8, 1>*  _jTemp;
			Eigen::Matrix<float, 8, 8>*  _hTemp;

			// optimization related:
			size_t	_maxIter;

	};

	inline MITracker::MITracker() :
		_numBins( 10 ),
		_jTemp( 0 ),
		_hTemp( 0 ),
		_maxIter( 10 )
	{
	}

	inline MITracker::~MITracker()
	{
		if( _jTemp )
			delete[] _jTemp;
		if( _hTemp )
			delete[] _hTemp;
	}

	inline void MITracker::updateInput( const Image & img )
	{
		size_t iter = 0;
		Matrix3f hinv;

		while( iter < _maxIter ){
			// warp back according to current pose
			const Matrix3f & h = _pose.pose();
			hinv = h.inverse();
			ITransform::apply( _warped, img, hinv );

			// calculate the online stuff:
			updateInputHistograms();
			updateDerivatives();
			//solveDeltaPose();	

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
		tmpf.convolve( _templateGradXY, IKernel::LAPLACE_33 );
	}

	inline void MITracker::updateTemplate( const Image& img, const Vector2f& pos )
	{
		Time t;
		_pose.pose().setHomography( 0.0f, 0.0f, 1.0f, 1.0f, pos.x, pos.y, 0.0f, 0.0f );
		_itemplate.reallocate( img );
		_itemplate = img;
		updateTemplateGradients();
		offlineTemplateDerivatives();

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
		// TODO: histogram of warped and joint histogram of warped and template
	}

	inline void MITracker::updateDerivatives()
	{
		Eigen::Matrix<float, 8, 1> currJac;
		Eigen::Matrix<float, 8, 8> currHess;

		size_t wStride;
		const uint8_t* ptr = _warped.map( &wStride );

		size_t height = _warped.height();
		size_t width  = _warped.width();

		_miJacobian.setZero();
		_miHessian.setZero();

		float jointHist = 1.0f;
		float tempHist = 1.0f;

		float normFactor = ( float )_numBins / 255.0f;
		float numSamples = width * height;
		float splineVal;
		for( size_t r = 0; r < _numBins; r++ ){
			for( size_t t = 0; t < _numBins; t++ ){
				const uint8_t* p = ptr;
				currJac.setZero();
				currHess.setZero();

				size_t iter = 0;
				for( size_t h = 0; h < height; h++ ){
					for( size_t w = 0; w < width; w++, iter++ ){
						splineVal = BSpline<float>::eval( t - p[ w ] * normFactor );
						currJac += splineVal * _jTemp[ iter * _numBins + r ];	
						currHess += splineVal * _hTemp[ iter * _numBins + r ];	
					}
					p += wStride; 
				}

				// normalize by number of pixels
				currJac *= numSamples;
				currHess *= numSamples;

				// TODO: evaluate the joint hist at (r, t)
				float tmp = 1 + Math::log( jointHist / tempHist );

				// sum the jacobians & the hessian
				_miJacobian += tmp * currJac;
				_miHessian  += currJac * currJac.transpose() * ( 1.0f / jointHist - 1.0f / tempHist ) + tmp * currHess;
			}
		}

		_warped.unmap( ptr );
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
		const uint8_t * i_ptr = _itemplate.map<uint8_t>( &iStride );
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
		const uint8_t* iptr = i_ptr;

		Eigen::Matrix<float, 1, 2> grad;
		Eigen::Matrix<float, 2, 2> hess;
		Eigen::Matrix<float, 1, 8> imagePoseDeriv;
		Eigen::Matrix<float, 8, 8> imagePoseDeriv2, wx, wy;

		float splineDeriv = 1.0f;
		float splineDeriv2 = 1.0f;

		size_t iter = 0;
		float normFactor = ( float )_numBins / 255.0f;
		float pixVal;
		for( size_t y = 0; y < _itemplate.height(); y++ ){
			p.y = y;
			for( size_t x = 0; x < _itemplate.width(); x++, iter++ ){
				// first order image derivatives
				grad[ 0 ] = gx[ x ];
				grad[ 1 ] = gy[ x ];

				// second order image derivatives
				hess( 0, 0 ) = gxx[ x ];
				hess( 0, 1 ) = hess( 1, 0) = gxy[ x ];
				hess( 1, 1 ) = gyy[ x ];

				p.x = x;
				_pose.screenJacobian( screenJac, p );
				_pose.screenHessian( wx, wy, p );

				imagePoseDeriv = grad * screenJac;
				
				// second order image pose derivative
				imagePoseDeriv2 = screenJac.transpose() * hess * screenJac + grad[ 0 ] * wx + grad[ 1 ] * wy; 
			
				pixVal = normFactor * iptr[ x ];
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
