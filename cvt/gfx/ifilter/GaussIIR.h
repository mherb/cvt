#ifndef IFILTER_GAUSSIIR_H
#define IFILTER_GAUSSIIR_H

#include <cvt/gfx/IFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt
{
	/* gaussian filter with infinite impulse response */
	class GaussIIR : public IFilter {
		public:
					GaussIIR();
					~GaussIIR();
			void	applyOpenCL( Image& dst, const Image& src, const Vector4f & n, const Vector4f & m, const Vector4f & d ) const;
			void	applyCPUf( Image& dst, const Image& src, const Vector4f & n, const Vector4f & m, const Vector4f & d ) const;
			void	applyCPUu8( Image& dst, const Image& src, const Vector4f & n, const Vector4f & m, const Vector4f & d ) const;
			void	apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;

		private:
			mutable CLKernel*	_kernelIIR;
			mutable CLKernel*	_kernelIIR2;
	};
}

#endif
