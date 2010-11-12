#ifndef CVTMUTEX_H
#define CVTMUTEX_H

#include <cvt/util/Exception.h>
#include <pthread.h>
#include <errno.h>

namespace cvt {
	class Mutex {
		friend class Condition;
		public:
			Mutex();
			~Mutex();
			void lock();
			void unlock();
			bool trylock();
		private:
			Mutex( const Mutex& t );

			pthread_mutex_t _tmutex;
	};

	inline Mutex::Mutex()
	{
		int err;
		err = pthread_mutex_init( &_tmutex, NULL );
		if( err )
			throw CVTException( err );
	}

	inline Mutex::~Mutex()
	{
		int err;
		err = pthread_mutex_destroy( &_tmutex );
		if( err )
			throw CVTException( err );
	}

	inline void Mutex::lock()
	{
		pthread_mutex_lock( &_tmutex );
	}

	inline bool Mutex::trylock()
	{
		int ret = pthread_mutex_trylock( &_tmutex );
		if( ret && ret != EBUSY )
			throw CVTException( ret );
		return ( ret == EBUSY );
	}

	inline void Mutex::unlock()
	{
		int err;
		err = pthread_mutex_unlock( &_tmutex );
		if( err )
			throw CVTException( err );
	}
}

#endif
