#include <cvt/gfx/IFilter.h>
#include <cvt/util/Plugin.h>
#include <cvt/util/PluginManager.h>

namespace cvt {
	static ParamInfoTyped<Image*> _pinput( "Input" );
	static ParamInfoTyped<float>  _plow( "Threshold low", 0.0f, 1.0f, 0.01f );
	static ParamInfoTyped<float>  _phigh( "Threshold high", 0.0f, 1.0f, 0.05f );
	static ParamInfoTyped<Image*> _poutput( "Output", false );
	static ParamInfoTyped<Image*> _psobelx( "Gradient X", false );
	static ParamInfoTyped<Image*> _psobely( "Gradient Y", false );

	static ParamInfo* _canny_params[] =
	{
		&_pinput,
		&_plow,
		&_phigh,
		&_poutput,
		&_psobelx,
		&_psobely
	};

	class Canny : public IFilter
	{
		public:
			Canny();
			~Canny();
			void apply( const ParamSet* attribs, IFilterType iftype ) const;

		private:
			Canny( const Canny& );
	};

	Canny::Canny() : IFilter( "Canny", _canny_params, 6, IFILTER_CPU )
	{
	}

	Canny::~Canny()
	{
	}

	void Canny::apply( const ParamSet* attribs, IFilterType iftype ) const
	{
	}

}

static void _init( cvt::PluginManager* pm )
{
	cvt::IFilter* canny = new cvt::Canny();
	pm->registerPlugin( canny );
}

extern "C" {
	cvt::PluginInfo _cvtplugin = { 0x43565450, 0, 1, ( void ( * )( cvt::PluginManager* ) ) _init };
}
