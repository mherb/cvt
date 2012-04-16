#ifndef CVT_PARAMSETVIEW_H
#define CVT_PARAMSETVIEW_H

#include <cvt/gui/WidgetContainer.h>
#include <cvt/util/ParamSet.h>

#include <ParamView.h>

#include <vector>

namespace cvt {

	class ParamSetView : public WidgetContainer
	{
            public:
                ParamSetView( ParamSet& pset );

                ~ParamSetView();

            private:
                std::vector<ParamView*> _paramViews;
	};
}

#endif
