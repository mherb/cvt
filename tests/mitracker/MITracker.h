#include <cvt/gfx/Image.h>

#include "PoseHomography.h"

namespace cvt {
	class MITracker {
		public:
			MITracker();
			~MITracker();

			void	updateInput( const Image& img );
			void	updateTemplate( const Image& img );

		private:
			void	updateInputGradients( const Image& icurrent);
			void	updateInputHistograms( const Image& icurrent );
			void	updateTemplateGradients();
			void	updateTemplateHistograms();
			void	calculatePoseDerivatives();
			void	solveDeltaPose();

			Image	_itemplate;
			PoseHomography<double> _pose;
	};
}
