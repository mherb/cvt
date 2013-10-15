#ifndef CVTCONDITION_H
#define CVTCONDITION_H

#include <cvt/util/Exception.h>
#include <pthread.h>
#include <errno.h>

namespace cvt {
	class Condition {
		public:
			Condition();
			~Condition();
			void wait( Mutex& mtx );
			void notify();
			void notifyAll();
		private:
			Condition( const Condition& c );
			pthread_cond_t _tcond;
	};

	inline Condition::Condition()
	{
		int err;
		err = pthread_cond_init( &_tcond, NULL );
		if( err )
			throw CVTException( err );
	}

	inline Condition::~Condition()
	{
		int err;
		err = pthread_cond_destroy( &_tcond );
		if( err )
			throw CVTException( err );
	}

	inline void Condition::wait( Mutex& mtx )
	{
		int err;
		err = pthread_cond_wait( &_tcond, &mtx._tmutex );
		if( err )
			throw CVTException( err );
	}

	inline void Condition::notify()
	{
		int err;
		err = pthread_cond_signal( &_tcond );
		if( err )
			throw CVTException( err );
	}

	inline void Condition::notifyAll()
	{
		int err;
		err =  pthread_cond_broadcast( &_tcond );
		if( err )
			throw CVTException( err );
	}
}

#endif
