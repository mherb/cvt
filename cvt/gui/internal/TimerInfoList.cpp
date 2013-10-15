#include <cvt/gui/internal/TimerInfoList.h>
#include <cvt/gui/internal/TimerInfo.h>

namespace cvt {
		TimerInfoList::TimerInfoList()
		{
		}

		TimerInfoList::~TimerInfoList()
		{
			// FIXME: do cleanup
		}


		void TimerInfoList::insertTimer( std::list<TimerInfo*>* list, TimerInfo* t )
		{
			for( std::list<TimerInfo*>::reverse_iterator rit = list->rbegin() ; rit != list->rend(); ++rit ) {
				if( ( *rit )->compare( t ) < 0  ) {
					_timers.insert( rit.base(), t );
					return;
				}
			}
			_timers.push_front( t );
		}

		uint32_t TimerInfoList::registerTimer( size_t intervalms, TimeoutHandler* th )
		{
			TimerInfo* ti = new TimerInfo( intervalms, th );
			insertTimer( &_timers, ti );
			return ti->id();
		}

		void TimerInfoList::unregisterTimer( uint32_t id )
		{
			for( std::list<TimerInfo*>::iterator it =_timers.begin() ; it != _timers.end(); ++it ) {
				if( ( *it )->id() == id  ) {
					TimerInfo* ti = *it;
					_timers.erase( it );
					delete ti;
					return;
				}
			}
		}

		int TimerInfoList::nextTimeout()
		{
			if( _timers.empty() )
				return -1;
			const TimerInfo* front = _timers.front();
			Time current;
			double t = ( front->timeout() - current );
			if( t > 0.0 )
				return ( int ) t;
			else
				return 0;
		}

		void TimerInfoList::handleTimers( )
		{
			Time now;
			std::list<TimerInfo*> tmplist;

			while( !_timers.empty() ) {
				if( ( _timers.front() )->timeout().compare( now ) < 0 ) {
					tmplist.splice( tmplist.begin(), _timers, _timers.begin() );
				} else
					break;
			}
			while( !tmplist.empty() ) {
					TimerInfo* tinfo = tmplist.back();
					tmplist.pop_back();
					tinfo->_th->onTimeout();
					tinfo->nextTimeout();
					insertTimer( &_timers, tinfo );
			}
		}

}
