#ifndef CVT_SIGNAL_H
#define CVT_SIGNAL_H

#include <cvt/util/Delegate.h>
#include <list>

namespace cvt {
	template<typename T> class Signal;

	template<typename T>
		class Signal
		{
			typedef typename std::list<Delegate<void ( T )> > ListType;
			typedef typename std::list<Delegate<void ( T )> >::iterator ListTypeIterator;

			public:
				Signal() {};
				~Signal() {};
				void add( Delegate<void ( T )>& d ) { _delegates.push_back( d ); }
				void remove( Delegate<void ( T )>& d ) { _delegates.remove( d ); };
				void notify( T arg );
				void notify( );

			private:
				ListType _delegates;
		};

	template<typename T>
		inline void Signal<T>::notify( T arg )
		{
			for( ListTypeIterator it = _delegates.begin(); it != _delegates.end(); ++it ) {
				it->operator()( arg );
			}
		}

	template<>
		class Signal<void>
		{
			typedef std::list<Delegate<void ( )> > ListType;
			typedef std::list<Delegate<void ( )> >::iterator ListTypeIterator;

			public:
				Signal() {};
				~Signal() {};
				void add( Delegate<void ( )>& d ) { _delegates.push_back( d ); }
				void remove( Delegate<void ( )>& d ) { _delegates.remove( d ); };
				void notify( void );

			private:
				ListType _delegates;
		};


		inline void Signal<void>::notify( void )
		{
			for( Signal<void>::ListTypeIterator it = _delegates.begin(); it != _delegates.end(); ++it ) {
				it->operator()( );
			}
		}

}


#endif
