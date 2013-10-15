#ifndef CVTTQUEUE_H
#define CVTTQUEUE_H

#include <deque>
#include <cvt/util/Mutex.h>
#include <cvt/util/Condition.h>

namespace cvt {
	template<typename T>
	class TQueue {
		public:
			TQueue() {};
			~TQueue() {};
			void enqueue( T a );
			T waitNext();

		private:
			std::deque<T> _queue;
			Mutex _mutex;
			Condition _cond;
	};

	template<typename T>
	void TQueue<T>::enqueue( T e )
	{
		_mutex.lock();
		_queue.push_back( e );
		_cond.notify();
		_mutex.unlock();
	}

	template<typename T>
	T TQueue<T>::waitNext()
	{
		T ret;
		_mutex.lock();
		while( _queue.empty() )
			_cond.wait( _mutex );
		ret = _queue.front();
		_queue.pop_front();
		_mutex.unlock();
		return ret;
	}

}



#endif
