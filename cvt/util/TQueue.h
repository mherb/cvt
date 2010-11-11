#ifndef CVTTQUEUE_H
#define CVTTQUEUE_H

#include <dequeue>

namespace cvt {
	template<typename T>
	class TQueue {
		public:
			TQueue() {};
			~TQueue() {};
			void enqueue( T a );
			T waitNext();

		private:
			std::dequeue<T> _queue;
			Mutex _mutex;
			Condition _cond;
	};

	void TQueue<T>::enqueue( T e )
	{
		_mutex.lock();
		_queue.push_back( e );
		_mutex.unlock();
		_cond.notify();
	}

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
