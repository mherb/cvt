/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/io/IOSelect.h>
#include <cvt/io/IOHandler.h>
#include <cvt/math/Math.h>


namespace cvt {

	IOSelect::IOSelect()
	{
	}

	IOSelect::~IOSelect()
	{

	}

	int IOSelect::handleIO( ssize_t ms )
	{
		int maxfd = 0;
		int numfd, ret, active;

		if( _handlers.empty() )
			return 0;


		FD_ZERO( &_readfds );
		FD_ZERO( &_writefds );
		FD_ZERO( &_execeptfds );

		for( std::list<IOHandler*>::iterator it = _handlers.begin(), end = _handlers.end(); it != end; ++it ) {
			IOHandler* ioh = *it;
			if( ioh->_read || ioh->_write || ioh->_except ) {
				maxfd = Math::max( maxfd, ioh->_fd );
				if( ioh->_read )
					FD_SET( ioh->_fd, &_readfds );
				if( ioh->_write )
					FD_SET( ioh->_fd, &_writefds );
				if( ioh->_except )
					FD_SET( ioh->_fd, &_execeptfds );
			}
		}

		if( ms < 0 ) {
			ret = pselect( maxfd + 1, &_readfds, &_writefds, &_execeptfds, NULL, NULL );
		} else {
			msToTimespec( ms, _timeout );
			ret = pselect( maxfd + 1, &_readfds, &_writefds, &_execeptfds, &_timeout, NULL );
		}
		/* FIXME: do error handling */
		if( ret <= 0 )
			return ret;

		numfd = ret;

		for( std::list<IOHandler*>::iterator it = _handlers.begin(), end = _handlers.end(); it != end; ++it ) {
			IOHandler* ioh = *it;
			if( ioh->_read || ioh->_write || ioh->_except ) {
				active = 0;
				if( ioh->_read && FD_ISSET( ioh->_fd, &_readfds ) ) {
					ioh->onDataReadable();
					active = 1;
				}
				if( ioh->_write && FD_ISSET( ioh->_fd, &_writefds ) ) {
					ioh->onDataWriteable();
					active = 1;
				}
				if( ioh->_except && FD_ISSET( ioh->_fd, &_execeptfds ) ) {
					ioh->onException();
					active = 1;
				}
				numfd -= active;
				if( !numfd )
					break;
			}
		}

		return ret;
	}
}
