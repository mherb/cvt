#include <cvt/io/audio/AudioDevice.h>
#include <cvt/util/PluginManager.h>

namespace cvt {


	void AudioDevice::devices( std::vector<AudioDevice*>& devices )
	{
		PluginManager::instance().getAudioDevices( devices );
	}

}
