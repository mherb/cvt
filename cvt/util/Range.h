/*
 *  Range.h
 *  CVTools
 *
 *  Created by Sebastian Klose on 16.08.10.
 *  Copyright 2010. All rights reserved.
 *
 */

#ifndef RANGE_H
#define RANGE_H

#include <iostream>

namespace cvt {
	
	
template <typename T> 
class Range 
{
public:
	Range(T min, T max);
	
	T min;
	T max;	
};
	
template<typename T>
inline std::ostream& operator<<(std::ostream &out, const Range<T> &range)
{
	return out << "[" << range.min << ":" << range.max << "]" << std::endl;
}
	
typedef Range<float> Rangef;
typedef Range<double> Ranged;
typedef Range<int> Rangei;
	
template<typename T>
Range<T>::Range(T min, T max):
	min(min), max(max)
{
}
	
}

#endif

