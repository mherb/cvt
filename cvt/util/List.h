#ifndef CVT_LIST_H
#define CVT_LIST_H

#include <stdint.h>
#include <string.h>

namespace cvt {

	template<typename T>
	class List {
		private:
			struct Node {
				Node( const T& data, Node* next = NULL , Node* prev = NULL ) : _next( next ), _prev( prev ), _data( data ) {}
				Node( const Node& it ) : _next( it._next ), _prev( it._prev ), _data( it._data ) {}
				Node& operator=( const Node& it ) { if( &it != this ) { _next = it._next; _prev = it._prev, _data = it._data; } return *this; }

				Node* _next;
				Node* _prev;
				T _data;
			};


		public:
			List();
			~List();
			List( const List<T>& list );
			List& operator=( const List<T>& list );

			void append( const T& data );
			void prepend( const T& data );
			void clear();
			size_t size() const;

			class Iterator {
				friend class List;
				public:
					Iterator() : _it( NULL ) {}
					Iterator( const Iterator& it ) : _it( it._it ) {}
					Iterator& operator=( const Iterator& it ) { _it = it._it; return *this; }

					bool operator==( const Iterator& it ) { return _it == it._it; }
					bool operator!=( const Iterator& it ) { return _it != it._it; }

					Iterator& operator++() { _it = _it->_next; return *this; }
					Iterator operator++( int ) { Iterator ret( *this ); _it = _it->_next; return ret; }
					Iterator& operator--() { _it = _it->_prev; return *this; }
					Iterator operator--( int ) { Iterator ret( *this ); _it = _it->_prev; return ret; }

					T& operator*() { return _it->_data; }

				private:
					Iterator( Node* it ) : _it( it ) {}
					Node* _it;
			};

			Iterator begin() { return Iterator( _head ); }
			Iterator end() { return Iterator(); }

		private:
			Node*	_head;
			Node*	_tail;
			size_t	_size;
	};

	template<typename T>
	inline List<T>::List() : _head( NULL ), _tail( NULL ), _size( 0 )
	{
	}

	template<typename T>
	inline List<T>::~List()
	{
		clear();
	}

	template<typename T>
	inline List<T>::List( const List<T>& list )
	{
		Node* it = list._head;
		while( it ) {
			append( it->_data );
			it = it->_next;
		}
	}

	template<typename T>
	inline size_t List<T>::size() const
	{
		return _size;
	}

	template<typename T>
	inline List<T>& List<T>::operator=( const List<T>& list )
	{
		if( this != &list ) {
			clear();
			Node* it = list._head;
			while( it ) {
				append( it->_data );
				it = it->_next;
			}
		}
		return *this;
	}

	template<typename T>
	inline void List<T>::append( const T& data )
	{
		if( !_head ) {
			Node* node = new Node( data );
			_head = node;
			_tail = node;
		} else {
			Node* node = new Node( data, NULL, _tail );
			_tail->_next = node;
			_tail = node;
		}
		_size++;
	}

	template<typename T>
	inline void List<T>::prepend( const T& data )
	{
		if( !_head ) {
			Node* node = new Node( data );
			_head = node;
			_tail = node;
		} else {
			Node* node = new Node( data, _head, NULL );
			_head->_prev = node;
			_head = node;
		}
		_size++;
	}

	template<typename T>
	inline void List<T>::clear()
	{
		Node* it = _head;
		while( it ) {
			Node* c = it;
			it = it->_next;
			delete c;
		}
		_head = NULL;
		_tail = NULL;
		_size = 0;
	}
}

#endif
