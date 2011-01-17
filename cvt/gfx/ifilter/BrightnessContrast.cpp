#include <cvt/gfx/ifilter/BrightnessContrast.h>
#include <cvt/cl/kernel/BC.h>

namespace cvt {

	static ParamInfo* _params[ 4 ] = {
		new ParamInfoTyped<Image*>( "Input", true /* inputParam */ ),
		new ParamInfoTyped<Image*>( "Output", false ),
		new ParamInfoTyped<float>( "Brightness", -1.0f /* min */, 1.0f /* max */, 0.0f /* default */, true ),
		new ParamInfoTyped<float>( "Contrast", -1.0f /* min */, 1.0f /* max */, 0.0f /* default */, true )
	};

	BrightnessContrast::BrightnessContrast() : IFilter( "BrightnessContrast", _params, 4, IFILTER_OPENCL ), _kernelBC( 0 )
	{
	}

	BrightnessContrast::~BrightnessContrast()
	{
		if( _kernelBC )
			delete _kernelBC;
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
		// TODO: THIS IS A HACK -> we change members of a const object!
		if( _kernelBC == 0 ){
			std::string log;
			_kernelBC = new CLKernel();
			_kernelBC->build( "BC", _BC_source, strlen( _BC_source ), log );
		}

		size_t w, h;
		w = src.width();
		h = src.height();
		_kernelBC->setArg( 0, dst );
		_kernelBC->setArg( 1, src );
		_kernelBC->setArg( 2, brightness );
		_kernelBC->setArg( 3, contrast );
		_kernelBC->run( cl::NDRange( w, h ), cl::NDRange( 8, 8 ) );
	}

}
