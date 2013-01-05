/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_AUDIOIOHANDLER_H
#define CVT_AUDIOIOHANDLER_H

namespace cvt
{
	class AudioOutput;
	class AudioInput;

	class AudioIOHandler
	{
		public:
			virtual void handle( AudioOutput* output ) = 0;
			virtual void handle( AudioInput* output ) = 0;
	};
}

#endif
