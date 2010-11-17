#ifndef CVT_SIGNAL_H
#define CVT_SIGNAL_H

#include <cvt/util/Delegate.h>
#include <list>

namespace cvt {
	template<typename T>
		class Signal
		{
			typedef typename std::list<Delegate<void ( T )>*> ListType;
			typedef typename std::list<Delegate<void ( T )>*>::iterator ListTypeIterator;

			public:
				Signal() {};
				~Signal() {};
				void add( Delegate<void ( T )>* d ) { _delegates.push_back( d ); }
				void remove( Delegate<void ( T )>* d ) { _delegates.remove( d ); };
				void notify( T arg );

			private:
				ListType _delegates;
		};

	template<typename T>
		inline void Signal<T>::notify( T arg )
		{
			for( ListTypeIterator it = _delegates.begin(); it != _delegates.end(); ++it ) {
				( *it )->operator()( arg );
			}
		}
}


#endif