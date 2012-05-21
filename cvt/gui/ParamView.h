/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_PARAMVIEW_H
#define CVT_PARAMVIEW_H

#include <cvt/gui/WidgetContainer.h>
#include <cvt/gui/Label.h>
#include <cvt/gui/Slider.h>
#include <cvt/util/ParamSet.h>

namespace cvt
{

    class ParamView : public WidgetContainer
    {
        public:
            ParamView( ParamSet& pset, size_t paramIdx );

        private:
            ParamSet&           _pset;
            size_t              _paramIdx;
            size_t              _paramHandle;
            Label               _nameLabel;
            Label               _valueLabel;
            Slider<float>       _slider;

            void handleSliderChange( float v );
            void initializeSliderRanges();
    };

}

#endif
