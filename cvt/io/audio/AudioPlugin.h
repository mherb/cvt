/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_AUDIOPLUGIN_H
#define CVT_AUDIOPLUGIN_H

#include <cvt/util/String.h>
#include <cvt/util/Plugin.h>
#include <cvt/io/audio/AudioDevice.h>

namespace cvt {
	class AudioPlugin : public Plugin
	{
		public:
			virtual void		  devices( std::vector<AudioDevice*>& devices ) = 0;
			const String&		  name() const { return _name; };

		protected:
			AudioPlugin( const String& name ) : Plugin( PLUGIN_AUDIO ), _name( name ) {};

			String _name;

	};
}

#endif


