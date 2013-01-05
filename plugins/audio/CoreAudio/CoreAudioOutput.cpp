/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */

#include <string.h>
#include "CoreAudioOutput.h"

namespace cvt
{

	CoreAudioOutput::CoreAudioOutput( const CoreAudioDevice& device, const AudioFormat& format, AudioIOHandler& handler ) :
		_id( device.deviceID() ),
		_format( format ),
		_handler( handler )
	{
	}

	CoreAudioOutput::~CoreAudioOutput()
	{
	}

	void CoreAudioOutput::start()
	{
	}

	void CoreAudioOutput::stop()
	{
	}

	void CoreAudioOutput::write( const AudioBuffer& buffer )
	{
	}

	void CoreAudioOutput::setVolume( float volume )
	{
	}

	float CoreAudioOutput::volume() const
	{
		return 0.0f;
	}

	const AudioFormat& CoreAudioOutput::format() const
	{
		return _format;
	}



}
