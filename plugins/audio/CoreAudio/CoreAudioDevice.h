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

}

#endif
