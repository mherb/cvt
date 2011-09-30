#include <cvt/gfx/Image.h>
#include <cvt/gfx/IKernel.h>
#include <cvt/gfx/ifilter/ITransform.h>

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

		private:
			void	updateInputGradients( const Image& icurrent );
			void	updateInputHistograms( const Image& icurrent );

			void	updateTemplateGradients();
			void	updateTemplateHistograms();
			void	calculatePoseDerivatives();

			void	solveDeltaPose();

			Image	_itemplate;
			Image	_templateGradX;
			Image	_templateGradY;
			Image	_templateGradXX;
			Image	_templateGradYY;
			Image	_templateGradXY;

			Image	_currGradX;
			Image	_currGradY;
			Image	_currGradXX;
			Image	_currGradYY;
			Image	_currGradXY;

			// backprojection of image to template space
			Image	_warped;
			PoseHomography<float> _pose;
	};

	inline MITracker::MITracker()
	{
	}

	inline MITracker::~MITracker()
	{
	}

	inline void MITracker::updateInput( const Image & img )
	{
		updateInputGradients( img );
	}

	inline void MITracker::updateInputGradients( const Image & img )
	{
		const Matrix3f & h = _pose.pose();
		Matrix3f hinv = h.inverse();
		ITransform::apply( _warped, img, hinv );

		Image tmpf;
		_warped.convert( tmpf, IFormat::GRAY_FLOAT );
		
		tmpf.convolve( _currGradX, IKernel::HAAR_HORIZONTAL_3 );
		tmpf.convolve( _currGradY, IKernel::HAAR_VERTICAL_3 );
		tmpf.convolve( _currGradXX, IKernel::LAPLACE_XX );
		tmpf.convolve( _currGradYY, IKernel::LAPLACE_YY );
		tmpf.convolve( _currGradXY, IKernel::LAPLACE_33 );
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
		_pose.pose().setHomography( 0.0f, 0.0f, 1.0f, 1.0f, pos.x, pos.y, 0.0f, 0.0f );
		_itemplate.reallocate( img );
		_itemplate = img;
		updateTemplateGradients();

		// reallocate the backwarped sizes
		_warped.reallocate( _itemplate );
		_currGradX.reallocate( _warped.width(), _warped.height(), IFormat::GRAY_FLOAT );
		_currGradY.reallocate( _warped.width(), _warped.height(), IFormat::GRAY_FLOAT );
		_currGradXX.reallocate( _warped.width(), _warped.height(), IFormat::GRAY_FLOAT );
		_currGradXY.reallocate( _warped.width(), _warped.height(), IFormat::GRAY_FLOAT );
		_currGradYY.reallocate( _warped.width(), _warped.height(), IFormat::GRAY_FLOAT );
	}

	inline const Matrix3f& MITracker::pose() const
	{
		return _pose.pose();
	}

}
