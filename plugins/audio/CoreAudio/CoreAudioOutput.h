/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_COREAUDIOOUTPUT_H
#define CVT_COREAUDIOOUTPUT_H

#include <cvt/io/audio/AudioOutput.h>

#include "CoreAudioDevice.h"

namespace cvt
{
	class CoreAudioDevice;

	class CoreAudioOutput : public AudioOutput
	{
		public:
			CoreAudioOutput( const CoreAudioDevice& device, const AudioFormat& format, AudioIOHandler& handler );
			~CoreAudioOutput();

			void			   start();
			void			   stop();
			void			   write( const AudioBuffer& buffer );

			void			   setVolume( float volume );
			float			   volume() const;

			const AudioFormat& format() const;

		private:
			static OSStatus audioIOProc( AudioDeviceID dev,
									     const AudioTimeStamp* inNow,
										 const AudioBufferList* inInputData,
										 const AudioTimeStamp* inInputTime,
										 AudioBufferList* outOutputData,
										 const AudioTimeStamp* inOutputTime,
										 void* ptr );

			static OSStatus audioDevProc( AudioDeviceID dev,
										  UInt32 line,
										  const AudioObjectPropertyAddress* pa,
										  void* ptr );


			AudioDeviceID	_id;
			AudioFormat		_format;
			AudioIOHandler& _handler;
	};
}


#endif
