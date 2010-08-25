#ifndef CALCFLOW_H
#define CALCFLOW_H
#include <cvt/gfx/Image.h>

void calcflow( cvt::Image& flow, cvt::Image& img1, cvt::Image& img2, cvt::Image* gt );

#endif
