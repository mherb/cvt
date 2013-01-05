/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_COREAUDIO_H
#define CVT_COREAUDIO_H

#include <cvt/io/audio/AudioPlugin.h>
#include <cvt/util/ScopedBuffer.h>

#include <CoreAudio/CoreAudio.h>

#include "CoreAudioDevice.h"


namespace cvt {
	class CoreAudio : public AudioPlugin
	{
		public:
			CoreAudio();
			~CoreAudio();

			void devices( std::vector<AudioDevice*>& devices );
	};

	inline CoreAudio::CoreAudio() : AudioPlugin( "CoreAudio" )
	{
	}

	inline CoreAudio::~CoreAudio()
	{
	}

	inline void CoreAudio::devices( std::vector<AudioDevice*>& devices )
	{
		 OSStatus            err = noErr;
	     UInt32              size = 0;
		 size_t				 numdevices;
		 AudioDeviceID*		 devs;

	    /* Get number of devices */
		AudioObjectPropertyAddress audioDevicesAddress = { kAudioHardwarePropertyDevices, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
		err = AudioObjectGetPropertyDataSize( kAudioObjectSystemObject, &audioDevicesAddress, 0, NULL, &size);
		if (err != noErr)
			return;

		numdevices = size / sizeof(AudioDeviceID);
		if( !numdevices )
			return;

		/* Allocate DeviceID array */
		ScopedBuffer<AudioDeviceID, true> devbuf( numdevices );
		devs = devbuf.ptr();

		/* Populate DeviceID array */
		err = AudioObjectGetPropertyData( kAudioObjectSystemObject, &audioDevicesAddress, 0, NULL, &size, devs );
		if (err != noErr)
			return;

		for( size_t i = 0; i < numdevices; i++) {
			devices.push_back( new CoreAudioDevice( devs[ i ] ) );
		}

	}

}

#endif
