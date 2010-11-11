#ifndef CVTTHREAD_H
#define CVTTHREAD_H

#include <cvt/util/Exception.h>
#include <pthread.h>

namespace cvt {
	template<typename T>
	class Thread {
		public:
			Thread();
			virtual ~Thread() {};
			void run( T* );
			virtual void execute( T* arg ) = 0;
			void join();

		private:
			Thread( const Thread& t );
			static void* _run( Thread* t );

			pthread_t _tid;
			pthread_attr_t _tattr;
			T* _arg;
	};

	template<typename T>
	Thread<T>::Thread() : _arg( NULL )
	{
		int err;
		err = pthread_attr_init( &_tattr );
		if( err )
			throw CVTException( err );
		err = pthread_attr_setdetachstate(&_tattr, PTHREAD_CREATE_JOINABLE );
		if( err )
			throw CVTException( err );
	}

	template<typename T>
	void Thread<T>::run( T* arg )
	{
		int err;
		_arg = arg;
		err = pthread_create( &_tid, &_tattr, ( void* (*)( void* ) )  Thread::_run, this );
		if( err )
			throw CVTException( err );
	}

	template<typename T>
	void Thread<T>::join( )
	{
		int err;
		err = pthread_join( _tid, NULL );
		if( err )
			throw CVTException( err );
	}

	template<typename T>
	void* Thread<T>::_run( Thread* t )
	{
		t->execute( t->_arg );
		pthread_exit( NULL );
	}

}


#endif
