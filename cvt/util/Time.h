#ifndef CVT_TIME_H
#define CVT_TIME_H

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#ifndef _POSIX_TIMERS
#error "Posix timers not supported"
#endif
#ifndef _POSIX_MONOTONIC_CLOCK
#error "Posix monotonic clock not supported"
#endif

namespace cvt {

	class Time {
		public:
			Time();
			Time( const Time& t );
			~Time();
			void reset();
			double elapsedSeconds() const;
			double elapsedMilliSeconds() const;
			double elapsedMicroSeconds() const;
			double ms() const;
			double operator+( const Time& t ) const;
			double operator-( const Time& t ) const;
			double operator+( double ms ) const;
			double operator-( double ms ) const;
			const Time& operator+=( size_t ms );
			int compare( const Time& t ) const;

		private:
			double timespecToMS( const struct timespec& ts ) const;
			double timespecToUS( const struct timespec& ts ) const;

			struct timespec _ts;
	};

	inline Time::Time()
	{
		reset();
	}

	inline Time::Time( const Time& t )
	{
		_ts.tv_sec = t._ts.tv_sec;
		_ts.tv_nsec = t._ts.tv_nsec;
	}

	inline Time::~Time() {}

	inline void Time::reset()
	{
		clock_gettime( CLOCK_MONOTONIC, &_ts );
	}

	inline double Time::timespecToMS( const struct timespec& ts ) const
	{
		return ( ( double ) ts.tv_sec ) * 1000.0 + ( ( double ) ts.tv_nsec ) * 0.000001;
	}

	inline double Time::timespecToUS( const struct timespec& ts ) const
	{
		return ( ( double ) ts.tv_sec ) * 1000000.0 + ( ( double ) ts.tv_nsec ) * 0.001;
	}

	inline double Time::elapsedSeconds() const
	{
		struct timespec ts2;
		clock_gettime( CLOCK_MONOTONIC, &ts2 );
		return ( double ) ts2.tv_sec - ( double ) _ts.tv_sec;
	}

	inline double Time::elapsedMilliSeconds() const
	{
		struct timespec ts2;
		clock_gettime( CLOCK_MONOTONIC, &ts2 );
		return timespecToMS( ts2 ) - timespecToMS( _ts );
	}

	inline double Time::elapsedMicroSeconds() const
	{
		struct timespec ts2;
		clock_gettime( CLOCK_MONOTONIC, &ts2 );
		return timespecToUS( ts2 ) - timespecToUS( _ts );
	}

	inline double Time::operator-( const Time& t ) const
	{
		return timespecToMS( _ts ) - timespecToMS( t._ts );
	}

	inline double Time::operator+( const Time& t ) const
	{
		return timespecToMS( _ts ) + timespecToMS( t._ts );
	}

	inline double Time::operator-( double ms ) const
	{
		return timespecToMS( _ts ) - ms;
	}

	inline double Time::operator+( double ms ) const
	{
		return timespecToMS( _ts ) + ms;
	}

	inline double Time::ms() const
	{
		return timespecToMS( _ts );
	}

	inline const Time& Time::operator+=( size_t ms )
	{
		long ns, sec;
		ldiv_t res;
		ns = ms * 1000000L + _ts.tv_nsec;
		res = ldiv( ns, 1000000000L );
		_ts.tv_sec += res.quot;
		_ts.tv_nsec = res.rem;
		return *this;
	}

	inline int Time::compare( const Time& t ) const
	{
		if ( _ts.tv_sec < t._ts.tv_sec)
			return -1;
		if ( _ts.tv_sec > t._ts.tv_sec)
			return 1;
		// FIXME: under/overflow
		return ( int ) ( _ts.tv_nsec - t._ts.tv_nsec );
	}
}

#endif
