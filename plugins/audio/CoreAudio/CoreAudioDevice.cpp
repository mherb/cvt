/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */

#include "CoreAudioDevice.h"
#include "CoreAudioOutput.h"

namespace cvt
{

	AudioOutput* CoreAudioDevice::createAudioOutput( const AudioFormat& format, AudioIOHandler& handler ) const
	{
		if( !( _features & AUDIO_DEVICE_OUTPUT ) )
			return NULL;

		return new CoreAudioOutput( *this, format, handler );
	}
}
