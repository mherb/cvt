/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_CWISEOP_H
#define CVT_CWISEOP_H

namespace cvt {
	struct CWiseAdd {
		template<typename T>
		static T eval( T a, T b ) { return a + b; }
	};

	struct CWiseSub {
		template<typename T>
		static T eval( T a, T b ) { return a - b; }
	};

	struct CWiseSubRev {
		template<typename T>
		static T eval( T a, T b ) { return b - a; }
	};

	struct CWiseMul {
		template<typename T>
		static T eval( T a, T b ) { return a * b; }
	};

	struct CWiseDiv {
		template<typename T>
		static T eval( T a, T b ) { return a / b; }
	};

	struct CWiseDivRev {
		template<typename T>
		static T eval( T a, T b ) { return b / a; }
	};


}

#endif
