/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/gfx/ifilter/BrightnessContrast.h>
#include <cvt/cl/kernel/BC.h>

namespace cvt {

	static ParamInfo* _params[ 4 ] = {
		new ParamInfoTyped<Image*>( "Input", true /* inputParam */ ),
		new ParamInfoTyped<Image*>( "Output", false ),
		new ParamInfoTyped<float>( "Brightness", -1.0f /* min */, 1.0f /* max */, 0.0f /* default */, true ),
		new ParamInfoTyped<float>( "Contrast", -1.0f /* min */, 1.0f /* max */, 0.0f /* default */, true )
	};

	BrightnessContrast::BrightnessContrast() : IFilter( "BrightnessContrast", _params, 4, IFILTER_OPENCL ), _kernelBC( _BC_source, "BC" )
	{
	}

	BrightnessContrast::~BrightnessContrast()
	{
	}

	void BrightnessContrast::apply( const ParamSet* set, IFilterType t ) const
	{
		Image * in = set->arg<Image*>( 0 );
		Image * out = set->arg<Image*>( 1 );
		float b = set->arg<float>( 2 );
		float c = set->arg<float>( 3 );

		switch ( t ) {
			case IFILTER_OPENCL:
				this->applyOpenCL( *out, *in, b, c );
				break;
			default:
				throw CVTException( "Not implemented" );
		}
	}

	void BrightnessContrast::applyOpenCL( Image& dst, const Image& src, float brightness, float contrast ) const
	{
		size_t w, h;
		w = src.width();
		h = src.height();
		_kernelBC.setArg( 0, dst );
		_kernelBC.setArg( 1, src );
		_kernelBC.setArg( 2, brightness );
		_kernelBC.setArg( 3, contrast );
		_kernelBC.run( CLNDRange( w, h ), CLNDRange( 8, 8 ) );
	}

}
