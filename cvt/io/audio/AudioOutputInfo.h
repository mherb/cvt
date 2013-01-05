/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_AUDIOOUTPUTOINFO_H
#define CVT_AUDIOOUTPUTOINFO_H

#include <cvt/util/String.h>
#include <cvt/util/Flags.h>

namespace cvt {

	enum AudioDeviceFlags {
		AUDIO_DEVICE_INPUT  = ( 1 << 0 ),
		AUDIO_DEVICE_OUTPUT = ( 1 << 1 )
	};

	CVT_ENUM_TO_FLAGS( AudioDeviceFlags, AudioDeviceFeatures )


	class AudioDevice {
		public:
			AudioDevice();
			AudioDevice( const AudioDevice& auoutinfo );
			~AudioDevice();

			const String& name() const;
			const AudioDeviceFeatures& features() const;

		private:
			String				_name;
			AudioDeviceFeatures _features;
	};
}

#endif
