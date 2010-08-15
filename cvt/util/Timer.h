#ifndef TIMER_H
#define TIMER_H

#include <boost/date_time/posix_time/posix_time.hpp>

namespace cvt {

    class Timer {
	public:
	    Timer() : time(boost::posix_time::microsec_clock::local_time()) {};

	    ~Timer()
	    {
	    }

	    void reset()
	    {
		time = boost::posix_time::microsec_clock::local_time();
	    }

	    double elapsedSeconds()
	    {
		return this->elapsedMicroSeconds()/1000000.0;
	    }

	    double elapsedMiliSeconds()
	    {
		return this->elapsedMicroSeconds()/1000.0;
	    }

	    double elapsedMicroSeconds()
	    {
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration t = now-time;
		return t.total_microseconds();
	    }

	private:
	    boost::posix_time::ptime time;
    };

}
#endif
