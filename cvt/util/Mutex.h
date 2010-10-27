#ifndef CVTMUTEX_H
#define CVTMUTEX_H

#include <pthread.h>

namespace cvt {
	class Mutex {
		public:
			Mutex();
			~Mutex();
			void lock();
			void unlock();
			bool trylock();
		private:
			pthread_mutex_t* _tmutex;
	};

	inline Mutex::Mutex()
	{
		pthread_mutex_init( &_tmutex, NULL );
	}

	inline Mutex::~Mutex()
	{
		pthread_mutex_destroy( &_tmutex );
	}

	inline void Mutex::lock()
	{
		pthread_mutex_lock( &_tmutex );
	}

	inline bool Mutex::trylock()
	{
		int ret = pthread_mutex_trylock( &_tmutex );
		return ( ret == EBUSY );
	}

	inline void Mutex::unlock()
	{
		pthread_mutex_unlock( &_tmutex );
	}
}

#endif
