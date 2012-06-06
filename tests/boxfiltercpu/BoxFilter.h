/*
 * Foo.h
 *
 *  Created on: Mar 19, 2012
 *      Author: popescu
 */

#ifndef BOX_FILTER_H_
#define BOX_FILTER_H_

#include <cvt/gfx/Image.h>

void BoxFilter( cvt::Image &dst, const cvt::Image &src, const cvt::Vector2i &size );

#endif /* BOX_FILTER_H_ */
