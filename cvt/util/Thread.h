#ifndef CVTTHREAD_H
#define CVTTHREAD_H

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
			static void* _run( Thread* t );

			pthread_t _tid;
			pthread_attr_t _tattr;
			T* _arg;
	};

	template<typename T>
	Thread<T>::Thread() : _arg( NULL )
	{
		pthread_attr_init( &_tattr );
		pthread_attr_setdetachstate(&_tattr, PTHREAD_CREATE_JOINABLE );
	}

	template<typename T>
	void Thread<T>::run( T* arg )
	{
		_arg = arg;
		pthread_create( &_tid, &_tattr, ( void* (*)( void* ) )  Thread::_run, this );
	}

	template<typename T>
	void Thread<T>::join( )
	{
		pthread_join( _tid, NULL );
	}

	template<typename T>
	void* Thread<T>::_run( Thread* t )
	{
		t->execute( t->_arg );
		pthread_exit( NULL );
	}

}


#endif
