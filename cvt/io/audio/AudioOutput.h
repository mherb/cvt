/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_AUDIOOUTPUT_H
#define CVT_AUDIOOUTPUT_H

#include <cvt/io/audio/AudioFormat.h>

namespace cvt {
	class AudioBuffer;

	class AudioOutput
	{
		public:
			virtual ~AudioOutput() {};

			virtual void			   start() = 0;
			virtual void			   stop() = 0;
			virtual void			   write( const AudioBuffer& buffer ) = 0;

			virtual void			   setVolume( float volume ) = 0;
			virtual float			   volume() const = 0;

			virtual const AudioFormat& format() const = 0;
	};

}

#endif
