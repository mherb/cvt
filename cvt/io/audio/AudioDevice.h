/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_AUDIODEVICE_H
#define CVT_AUDIODEVICE_H

#include <cvt/util/String.h>
#include <cvt/util/Flags.h>
#include <cvt/io/audio/AudioOutput.h>
#include <cvt/io/audio/AudioIOHandler.h>

namespace cvt {

	enum AudioDeviceFlags {
		AUDIO_DEVICE_INPUT		   = ( 1 << 0 ),
		AUDIO_DEVICE_OUTPUT		   = ( 1 << 1 ),
		AUDIO_DEVICE_DEFAULTINPUT  = ( 1 << 2 ),
		AUDIO_DEVICE_DEFAULTOUTPUT = ( 1 << 3 )
	};

	CVT_ENUM_TO_FLAGS( AudioDeviceFlags, AudioDeviceFeatures )

	class AudioFormat;

	class AudioDevice {
		public:
			virtual ~AudioDevice();

			virtual const String&			   name() const = 0;
			virtual const AudioDeviceFeatures& features() const = 0;
			virtual AudioOutput*			   createAudioOutput( const AudioFormat& format, AudioIOHandler& handler ) const = 0;

			static void devices( std::vector<AudioDevice*>& devs );

		protected:
			AudioDevice();

		private:
			AudioDevice( const AudioDevice& auoutinfo );
	};

	inline AudioDevice::AudioDevice()
	{
	}

	inline AudioDevice::~AudioDevice()
	{
	}

	inline std::ostream& operator<<( std::ostream &out, const AudioDevice& dev )
	{
		const AudioDeviceFeatures& f = dev.features();
		out << dev.name() << std::endl;
		out << "\tInput: " << ( ( f & AUDIO_DEVICE_INPUT )?"YES":"NO" ) << ( ( f & AUDIO_DEVICE_DEFAULTINPUT )?" ( DEFAULT )":"" ) << std::endl;
		out << "\tOutput: " << ( ( f & AUDIO_DEVICE_OUTPUT )?"YES":"NO" ) << ( ( f & AUDIO_DEVICE_DEFAULTOUTPUT )?" ( DEFAULT )":"" );
		return out;
	}
}

#endif
