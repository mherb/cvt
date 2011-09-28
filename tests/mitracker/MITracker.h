#include <cvt/gfx/Image.h>
#include <cvt/gfx/IKernel.h>

#include "PoseHomography.h"

namespace cvt {
	class MITracker {
		public:
			MITracker();
			~MITracker();

			void	updateInput( const Image& img );
			void	updateTemplate( const Image& img );

			const Image&	templateImage() const { return _itemplate; }
			const Image&	templateGradX() const { return _templateGradX; }
			const Image&	templateGradY() const { return _templateGradY; }

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

			Image	_currGradX;
			Image	_currGradY;

			// backprojection of image to template space
			Image	_warped;
			PoseHomography<double> _pose;
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
		_currGradX.reallocate( img.width(), img.height(), img.format() );
		_currGradY.reallocate( img.width(), img.height(), img.format() );
		img.convolve( _currGradX, IKernel::HAAR_HORIZONTAL_3 );
		img.convolve( _currGradY, IKernel::HAAR_VERTICAL_3 );

		// TODO: second derivatives
	}

	inline void MITracker::updateTemplateGradients()
	{
		_templateGradX.reallocate( _itemplate.width(), _itemplate.height(), _itemplate.format() );
		_templateGradY.reallocate( _itemplate.width(), _itemplate.height(), _itemplate.format() );
		_itemplate.convolve( _templateGradX, IKernel::HAAR_HORIZONTAL_3 );
		_itemplate.convolve( _templateGradY, IKernel::HAAR_VERTICAL_3 );
	}

	inline void MITracker::updateTemplate( const Image& img )
	{
		_itemplate.reallocate( img );
		_itemplate = img;
		updateTemplateGradients();
	}

}
