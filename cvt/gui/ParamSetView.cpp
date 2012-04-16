/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#include <cvt/gui/ParamSetView.h>
#include <cvt/gui/Slider.h>

namespace cvt
{

        ParamSetView::ParamSetView( ParamSet& pset )
        {
            _paramViews.resize( pset.size() );

            WidgetLayout wl;
            wl.setRelativeLeftRight( 0.01f, 0.99f );
            float relTop = 0.0f;
            float relStep = 1.0f / pset.size();
            for( size_t i = 0; i < pset.size(); i++ ){
                _paramViews[ i ] = new ParamView( pset, i );

                wl.setRelativeTopBottom( relTop, relTop + relStep );
                addWidget( _paramViews[ i ], wl );
                relTop += relStep;
            }
        }

        ParamSetView::~ParamSetView()
        {
            for( size_t i = 0; i < _paramViews.size(); i++ ){
                if( _paramViews[ i ] )
                    delete _paramViews[ i ];
            }
            _paramViews.clear();
        }
}
