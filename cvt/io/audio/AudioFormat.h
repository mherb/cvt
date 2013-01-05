/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_AUDIOFORMAT_H
#define CVT_AUDIOFORMAT_H

namespace cvt
{
	enum AudioSampleFormat {
		AUDIO_SAMPLE_INT8	= ( 1 << 0 ),
		AUDIO_SAMPLE_INT16  = ( 1 << 1 ),
		AUDIO_SAMPLE_INT32  = ( 1 << 2 ),
		AUDIO_SAMPLE_FLOAT  = ( 1 << 3 )
	};

#define AUDIO_SAMPLERATE_4000     4000
#define AUDIO_SAMPLERATE_5512     5512
#define AUDIO_SAMPLERATE_8000     8000
#define AUDIO_SAMPLERATE_9600     9600
#define AUDIO_SAMPLERATE_11025   11025
#define AUDIO_SAMPLERATE_22050   22050
#define AUDIO_SAMPLERATE_32000   32000
#define AUDIO_SAMPLERATE_44100   41000
#define AUDIO_SAMPLERATE_48000	 48000
#define AUDIO_SAMPLERATE_88200   88200
#define AUDIO_SAMPLERATE_96000   96000
#define AUDIO_SAMPLERATE_176000 176000
#define AUDIO_SAMPLERATE_192000 192000

	class AudioFormat
	{
		public:
			AudioFormat( size_t channels, size_t samplerate, AudioSampleFormat sampleformat );
			AudioFormat( const AudioFormat& format );
			~AudioFormat();

			const AudioFormat& operator=( const AudioFormat& format );
			bool			   operator==( const AudioFormat& format ) const;

			size_t			   channels() const;
			void			   setChannels( size_t channels );
			size_t			   sampleRate() const;
			void			   setSampleRate( size_t rate );
			AudioSampleFormat  sampleFormat() const;
			void			   setSampleFormat( AudioSampleFormat format );

			size_t			   memorySize( size_t samples ) const;

		private:
			size_t			  _channels;
			size_t			  _samplerate;
			AudioSampleFormat _sampleformat;
	};

	inline AudioFormat::AudioFormat( size_t channels, size_t samplerate, AudioSampleFormat format ) :
		_channels( channels ),
		_samplerate( samplerate ),
		_sampleformat( format )
	{
	}

	inline AudioFormat::~AudioFormat()
	{
	}

	inline AudioFormat::AudioFormat( const AudioFormat& format ) :
		_channels( format._channels ),
		_samplerate( format._samplerate ),
		_sampleformat( format._sampleformat )
	{
	}

	inline const AudioFormat& AudioFormat::operator=( const AudioFormat& format )
	{
		if( this == &format )
			return *this;

		_channels     = format._channels;
		_samplerate   = format._samplerate;
		_sampleformat = format._sampleformat;
		return *this;
	}

	inline bool AudioFormat::operator==( const AudioFormat& other ) const
	{
		return _channels == other._channels && _samplerate == other._samplerate && _sampleformat == other._sampleformat;
	}

	inline size_t AudioFormat::channels() const
	{
		return _channels;
	}

	inline void AudioFormat::setChannels( size_t c )
	{
		_channels = c;
	}

	inline size_t AudioFormat::sampleRate() const
	{
		return _samplerate;
	}

	inline void AudioFormat::setSampleRate( size_t rate )
	{
		_samplerate = rate;
	}

	inline AudioSampleFormat AudioFormat::sampleFormat() const
	{
		return _sampleformat;
	}

	inline void AudioFormat::setSampleFormat( AudioSampleFormat sformat )
	{
		_sampleformat = sformat;
	}

	inline size_t AudioFormat::memorySize( size_t samples ) const
	{
		size_t bps;
		switch( _sampleformat ) {
			case AUDIO_SAMPLE_INT8:  bps = 1; break;
			case AUDIO_SAMPLE_INT16: bps = 2; break;
			case AUDIO_SAMPLE_INT32: bps = 4; break;
			case AUDIO_SAMPLE_FLOAT: bps = 4; break;
		}
		return _channels * bps * samples;
	}
}

#endif
