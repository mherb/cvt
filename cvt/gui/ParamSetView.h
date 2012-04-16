/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_PARAMSETVIEW_H
#define CVT_PARAMSETVIEW_H

#include <cvt/gui/WidgetContainer.h>
#include <cvt/util/ParamSet.h>

#include <cvt/gui/ParamView.h>

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
