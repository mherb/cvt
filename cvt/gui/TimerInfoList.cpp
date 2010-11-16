#include <cvt/gui/TimerInfoList.h>
#include <cvt/gui/TimerInfo.h>

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
					_mtx.unlock();
					return;
				}
			}
			_timers.push_front( t );
		}

		uint32_t TimerInfoList::registerTimer( size_t intervalms, EventTimer* t )
		{
			TimerInfo* ti = TimerInfo::create( intervalms, t );
			_mtx.lock();
			insertTimer( &_timers, ti );
			_mtx.unlock();
			return ti->id();
		}

		void TimerInfoList::unregisterTimer( uint32_t id )
		{
			_mtx.lock();
			for( std::list<TimerInfo*>::iterator it =_timers.begin() ; it != _timers.end(); ++it ) {
				if( ( *it )->id() == id  ) {
					( *it )->release();
					_timers.erase( it );
					_mtx.unlock();
					return;
				}
			}
			_mtx.unlock();
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

		void TimerInfoList::queueEvents( TQueue<std::pair< ::Window,Event*> >* queue )
		{
			ScopeLock lock( &_mtx );
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
					TimerEvent* event = tinfo->event();
					if( event ) {
						queue->enqueue( std::make_pair< ::Window, Event*>( 0, event ) );
					}
					insertTimer( &_timers, tinfo );
			}
		}

}
