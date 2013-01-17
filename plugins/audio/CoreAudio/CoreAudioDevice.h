/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_COREAUDIODEVICE_H
#define CVT_COREAUDIODEVICE_H

#include <cvt/util/Exception.h>
#include <cvt/io/audio/AudioDevice.h>
#include <CoreAudio/CoreAudio.h>

namespace cvt {
	class CoreAudioDevice : public AudioDevice
	{
		public:
			CoreAudioDevice( AudioDeviceID id );
			~CoreAudioDevice();

			const String&			   name() const;
			const AudioDeviceFeatures& features() const;
			AudioOutput*			   createAudioOutput( const AudioFormat& format, AudioIOHandler& handler ) const;

			AudioDeviceID			   deviceID() const;

		private:
			bool setOutputSampleRate( size_t samplerate );
			size_t getOutputSampleRate() const;

			bool setVolume( float value );
			float getVolume() const;

			bool isDefaultOutput() const;
			bool isDefaultInput() const;
			bool isInput() const;
			bool isOutput() const;

			AudioDeviceID		_id;
			String			    _name;
			AudioDeviceFeatures _features;
	};

	inline CoreAudioDevice::CoreAudioDevice( AudioDeviceID id ) : _id( id ), _name( "CoreAudio - ")
	{
		CFStringRef cfstr;
		UInt32 size;
		OSStatus err = noErr;

		AudioObjectPropertyAddress property = { kAudioObjectPropertyName,
                                          kAudioObjectPropertyScopeGlobal,
                                          kAudioObjectPropertyElementMaster };

		size = sizeof( CFStringRef );

		err = AudioObjectGetPropertyData( _id, &property, 0, NULL, &size, &cfstr );
		if ( err != noErr ) {
			throw CVTException( "CoreAudio error!" );
		}

		_name += CFStringGetCStringPtr( cfstr, CFStringGetSystemEncoding() );
		CFRelease( cfstr );

		if( isInput() ) {
			_features |= AUDIO_DEVICE_INPUT;
			if( isDefaultInput() )
				_features |= AUDIO_DEVICE_DEFAULTINPUT;

		}

		if( isOutput() ) {
			_features |= AUDIO_DEVICE_OUTPUT;
			if( isDefaultOutput() )
				_features |= AUDIO_DEVICE_DEFAULTOUTPUT;

		}

		if( _features & AUDIO_DEVICE_OUTPUT )
		{
			OSStatus err = noErr;
			UInt32 size = 0;
			AudioObjectPropertyAddress property = { kAudioDevicePropertyStreamConfiguration, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMaster };
			err = AudioObjectGetPropertyDataSize( _id, &property, 0, NULL, &size );
			if ( err != noErr )
				return;

			std::cout <<  size / sizeof( AudioBufferList ) << std::endl;

			AudioBufferList* buflist = new AudioBufferList[ size / sizeof( AudioBufferList ) ];
			err = AudioObjectGetPropertyData( _id, &property, 0, NULL, &size, buflist );
			if ( err != noErr )
				return;

			for( size_t i = 0, end = buflist->mNumberBuffers; i < end; i++ ) {
				std::cout << buflist->mBuffers[i].mNumberChannels << std::endl;
			}
			delete[] buflist;

		}
	}

	inline CoreAudioDevice::~CoreAudioDevice()
	{
	}

	inline const String& CoreAudioDevice::name() const
	{
		return _name;
	}

	inline const AudioDeviceFeatures& CoreAudioDevice::features() const
	{
		return _features;
	}


	inline AudioDeviceID CoreAudioDevice::deviceID() const
	{
		return _id;
	}


	inline bool CoreAudioDevice::isDefaultOutput() const
	{
		OSStatus err = noErr;
		AudioDeviceID id;
		UInt32 size = sizeof( AudioDeviceID );

		AudioObjectPropertyAddress property = { kAudioHardwarePropertyDefaultOutputDevice, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
		err = AudioObjectGetPropertyData( kAudioObjectSystemObject, &property, 0, NULL, &size, &id );
		if ( err != noErr )
			return false;
		return _id == id;
	}


	inline bool CoreAudioDevice::isDefaultInput() const
	{
		OSStatus err = noErr;
		AudioDeviceID id;
		UInt32 size = sizeof( AudioDeviceID );

		AudioObjectPropertyAddress property = { kAudioHardwarePropertyDefaultInputDevice, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
		err = AudioObjectGetPropertyData( kAudioObjectSystemObject, &property, 0, NULL, &size, &id );
		if ( err != noErr )
			return false;
		return _id == id;
	}

	inline bool CoreAudioDevice::isInput() const
	{
		OSStatus err = noErr;
		UInt32 size;
		AudioObjectPropertyAddress streamInput = { kAudioDevicePropertyStreams, kAudioDevicePropertyScopeInput, kAudioObjectPropertyElementMaster };
		err = AudioObjectGetPropertyDataSize( _id, &streamInput, 0, NULL, &size );
		if( err != noErr )
			return false;
		return ( size != 0 );
	}

	inline bool CoreAudioDevice::isOutput() const
	{
		OSStatus err = noErr;
		UInt32 size;
		AudioObjectPropertyAddress streamOutput = { kAudioDevicePropertyStreams, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMaster };
		err = AudioObjectGetPropertyDataSize( _id, &streamOutput, 0, NULL, &size );
		if( err != noErr )
			return false;
		return ( size != 0 );
	}

	inline size_t CoreAudioDevice::getOutputSampleRate() const
	{
		OSStatus err = noErr;
		AudioObjectPropertyAddress rateaddr = { kAudioDevicePropertyNominalSampleRate, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMaster };
		UInt32 size = sizeof( Float64 );
		Float64 rate;
		err = AudioObjectGetPropertyData( _id, &rateaddr, 0, NULL, &size, &rate );
		if( err != noErr )
			return 0;
		return rate;
	}

	inline bool CoreAudioDevice::setOutputSampleRate( size_t samplerate )
	{
		OSStatus err = noErr;
		AudioObjectPropertyAddress rateaddr = { kAudioDevicePropertyNominalSampleRate, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMaster };
		Float64 rate = ( Float64 ) samplerate;
		err = AudioObjectSetPropertyData( _id, &rateaddr, 0, NULL, sizeof( Float64 ), &rate );
		if( err != noErr )
			return false;
		return true;
	}

	inline bool CoreAudioDevice::setVolume( float value )
	{
		// TODO: this sucks - how to set channels?
		OSStatus err = noErr;
		AudioObjectPropertyAddress voladdr1 = { kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, 1 };
		AudioObjectPropertyAddress voladdr2 = { kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, 2 };
		Float32 vol = ( Float32 ) value;
		err = AudioObjectSetPropertyData( _id, &voladdr1, 0, NULL, sizeof( Float32 ), &vol );
		if( err != noErr )
			return false;
		err = AudioObjectSetPropertyData( _id, &voladdr2, 0, NULL, sizeof( Float32 ), &vol );
		if( err != noErr )
			return false;
		return true;
	}

	inline float CoreAudioDevice::getVolume() const
	{
		// TODO: this sucks - how to set channels?
		OSStatus err = noErr;
		AudioObjectPropertyAddress voladdr1 = { kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, 1 };
		AudioObjectPropertyAddress voladdr2 = { kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, 2 };
		float ret;

		UInt32 size = sizeof( Float32 );
		Float32 volume;
		err = AudioObjectGetPropertyData( _id, &voladdr1, 0, NULL, &size, &volume );
		if( err != noErr )
			return 0;
		std::cout << volume << std::endl;
		ret = volume;
		err = AudioObjectGetPropertyData( _id, &voladdr2, 0, NULL, &size, &volume );
		if( err != noErr )
			return 0;
		std::cout << volume << std::endl;
		ret += volume;
		return ret / 2;
	}

}

#endif
