/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_TIMEOUTHANDLER_H
#define CVT_TIMEOUTHANDLER_H

namespace cvt {
	class TimeoutHandler {
		public:
			TimeoutHandler() {};
			virtual ~TimeoutHandler() {};
			virtual void onTimeout() = 0;
	};
}

#endif
