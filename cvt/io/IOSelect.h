#ifndef CVT_IOSELECT_H
#define CVT_IOSELECT_H

#include <stdlib.h>
#include <sys/select.h>
#include <time.h>

#include <list>

namespace cvt {
	class IOHandler;

	class IOSelect {
		public:
			IOSelect();
			~IOSelect();
			int handleIO( ssize_t timeout_ms );
			void registerIOHandler( IOHandler* ioh );
			void unregisterIOHandler( IOHandler* ion );

		private:
			void msToTimespec( size_t ms, struct timespec& ts ) const;

			fd_set _readfds;
			fd_set _writefds;
			fd_set _execeptfds;
			struct timespec _timeout;
			std::list<IOHandler*> _handlers;
	};


	inline void IOSelect::registerIOHandler( IOHandler* ioh )
	{
		_handlers.push_back( ioh );
	}

	inline void IOSelect::unregisterIOHandler( IOHandler* ion )
	{
		_handlers.remove( ion );
	}


	inline void IOSelect::msToTimespec( size_t ms, struct timespec& ts ) const
	{
		long ns;
		ldiv_t res;
		ns = ms * 1000000L;
		res = ldiv( ns, 1000000000L );
		ts.tv_sec = res.quot;
		ts.tv_nsec = res.rem;
	}

}

#endif
