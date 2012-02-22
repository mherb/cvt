/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_IOHANDLER_H
#define CVT_IOHANDLER_H

#include <cvt/io/IOSelect.h>

namespace cvt {

	class IOHandler {
		friend class IOSelect;

		public:
			IOHandler( int fd = -1 );
			virtual ~IOHandler();
			virtual void onDataReadable();
			virtual void onDataWriteable();
			virtual void onException();
			void notifyReadable( bool b );
			void notifyWriteable( bool b );
			void notifyException( bool b );

		private:
			IOHandler( const IOHandler& );

			bool _read;
			bool _write;
			bool _except;
		protected:
			int _fd;
	};

	inline IOHandler::IOHandler( int fd ) : _read( false ), _write( false ), _except( false ), _fd( fd )
	{
	}

	inline IOHandler::~IOHandler()
	{
	}

	inline void IOHandler::notifyReadable( bool b )
	{
		if( _fd >= 0 )
			_read = b;
	}

	inline void IOHandler::notifyWriteable( bool b )
	{
		if( _fd >= 0 )
			_write = b;
	}

	inline void IOHandler::notifyException( bool b )
	{
		if( _fd >= 0 )
			_except = b;
	}

	inline void IOHandler::onDataReadable()
	{
		notifyReadable( false );
	}

	inline void IOHandler::onDataWriteable()
	{
		notifyWriteable( false );
	}

	inline void IOHandler::onException()
	{
		notifyException( false );
	}


}

#endif
