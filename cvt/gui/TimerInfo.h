#ifndef CVT_TIMERINFO_H
#define CVT_TIMERINFO_H

#include <cvt/util/Time.h>
#include <cvt/util/Mutex.h>
#include <cvt/gui/EventTimer.h>
#include <cvt/gui/event/TimerEvent.h>
#include <cvt/util/IDGenerator.h>

namespace cvt {

	class TimerInfo {
		public:
			static TimerInfo* create( size_t msinterval, EventTimer* et );
			void release();
			uint32_t id() const;
			void dispatch();
			TimerEvent* event();
			int compare( const TimerInfo* t ) const;
			const Time& timeout() const;

		private:
			TimerInfo( size_t msinterval, EventTimer* et );
			TimerInfo( const TimerInfo& ti );
			~TimerInfo();

			size_t _interval;
			bool _active;
			bool _pending;
			EventTimer* _et;
			uint32_t _id;
			Time _timeout;
			Mutex _mtx;

			static IDGenerator _idgen;
	};

	inline TimerInfo::TimerInfo( size_t msinterval, EventTimer* t ) : _interval( msinterval ), _active( true ), _pending( false ), _et( t )
	{
		_timeout.reset();
		_timeout += _interval;
		_id = _idgen.allocateID();
	}

	inline TimerInfo::~TimerInfo()
	{
		_idgen.releaseID( _id );
	}

	inline uint32_t TimerInfo::id() const
	{
		return _id;
	}

	inline int TimerInfo::compare( const TimerInfo* t ) const
	{
		return _timeout.compare( t->_timeout );
	}

	inline void TimerInfo::dispatch()
	{
		_mtx.lock();
		_pending = false;
		if( _active ) {
			_mtx.unlock();
			_et->timeoutEvent();
			return;
		} else {
			_mtx.unlock();
			delete this;
			return;
		}
	}

	inline const Time& TimerInfo::timeout() const
	{
		return _timeout;
	}

	inline TimerEvent* TimerInfo::event()
	{
		_mtx.lock();
		if( _pending ) {
			_timeout.reset();
			_timeout += _interval;
			_mtx.unlock();
			return NULL;
		}
		_pending = true;
		TimerEvent* ret = new TimerEvent( this );
		_timeout.reset();
		_timeout += _interval;
		_mtx.unlock();
		return ret;
	}

	inline void TimerInfo::release()
	{
		_mtx.lock();
		_active = false;
		if( !_pending ) {
			_mtx.unlock();
			delete this;
			return;
		}
		_mtx.unlock();
	}

}

#endif
