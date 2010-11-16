#ifndef CVT_TIMERINFO_H
#define CVT_TIMERINFO_H

#include <cvt/util/Time.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/util/IDGenerator.h>

namespace cvt {

	class TimerInfo {
		friend class TimerInfoList;

		public:
			TimerInfo( size_t msinterval, TimeoutHandler* handler );
			TimerInfo( const TimerInfo& ti );
			uint32_t id() const;
			int compare( const TimerInfo* t ) const;
			void nextTimeout();
			const Time& timeout() const;

		private:
			~TimerInfo();

			size_t _interval;
			uint32_t _id;
			Time _timeout;
			TimeoutHandler* _th;

			static IDGenerator _idgen;
	};

	inline TimerInfo::TimerInfo( size_t msinterval, TimeoutHandler* handler ) : _interval( msinterval ), _th( handler )
	{
		_id = _idgen.allocateID();
		nextTimeout();
	}

	inline void TimerInfo::nextTimeout()
	{
		_timeout.reset();
		_timeout += _interval;
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

	inline const Time& TimerInfo::timeout() const
	{
		return _timeout;
	}
}

#endif
