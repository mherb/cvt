/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_LIST_H
#define CVT_LIST_H

#include <stdint.h>
#include <string.h>
#include <iostream>

namespace cvt {

	template<typename T>
	class List {
		private:
			struct NodeBase {
				NodeBase( NodeBase* next = NULL , NodeBase* prev = NULL ) : _next( next ), _prev( prev ) {}
				NodeBase( const NodeBase& it ) : _next( it._next ), _prev( it._prev )  {}
				NodeBase& operator=( const NodeBase& it ) { if( &it != this ) { _next = it._next; _prev = it._prev; } return *this; }

				NodeBase* _next;
				NodeBase* _prev;
			};

			struct Node : public NodeBase {
				Node( const T& data, NodeBase* next = NULL, NodeBase* prev = NULL ) : NodeBase( next, prev ), _data( data ) {}
				Node( const Node& node ) : NodeBase( node._next, node._prev ), _data( node._data ) {}
				Node& operator=( const Node& it ) { if( &it != this ) { NodeBase::_next = it._next; NodeBase::_prev = it._prev; _data = it._data; } return *this; }

				T _data;
			};


		public:
			class Iterator;
			class ReverseIterator;

			List();
			~List();
			List( const List<T>& list );
			List& operator=( const List<T>& list );

			void append( const T& data );
			void prepend( const T& data );
			void clear();
			size_t size() const;
			bool isEmpty() const;

			T& front();
			T& back();
			const T& front() const;
			const T& back() const;

			Iterator find( const T& data ) const;
			template<typename T2> Iterator find( const T2& data ) const;

			Iterator begin() { return Iterator( _anchor._next ); }
			Iterator end() { return Iterator( &_anchor ); }
			Iterator remove( Iterator it );

			Iterator rbegin() { return Iterator( _anchor._prev ); }
			Iterator rend() { return Iterator( &_anchor ); }
			Iterator remove( ReverseIterator it );

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

					T& operator*() { return ( ( Node* ) _it )->_data; }

				private:
					Iterator( NodeBase* it ) : _it( it ) {}
					NodeBase* _it;
			};

			class ReverseIterator {
				friend class List;
				public:
					ReverseIterator() : _it( NULL ) {}
					ReverseIterator( const ReverseIterator& it ) : _it( it._it ) {}
					ReverseIterator& operator=( const ReverseIterator& it ) { _it = it._it; return *this; }

					bool operator==( const ReverseIterator& it ) { return _it == it._it; }
					bool operator!=( const ReverseIterator& it ) { return _it != it._it; }

					ReverseIterator& operator++() { _it = _it->_prev; return *this; }
					ReverseIterator operator++( int ) { ReverseIterator ret( *this ); _it = _it->_prev; return ret; }
					ReverseIterator& operator--() { _it = _it->_next; return *this; }
					ReverseIterator operator--( int ) { ReverseIterator ret( *this ); _it = _it->_next; return ret; }

					T& operator*() { return ( ( Node* ) _it )->_data; }

				private:
					ReverseIterator( NodeBase* it ) : _it( it ) {}
					NodeBase* _it;
			};


		private:
			void unlinkNode( NodeBase* node );

			NodeBase  _anchor;
			size_t	  _size;
	};

	template<typename T>
	inline List<T>::List() : _anchor( &_anchor, &_anchor ), _size( 0 )
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
		clear();
		Node* it = list._anchor._next;
		while( it != &list._anchor ) {
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
	inline bool List<T>::isEmpty() const
	{
		return _size == 0;
	}

	template<typename T>
	inline T& List<T>::front()
	{
		return ( ( Node* ) _anchor._next )->_data;
	}

	template<typename T>
	inline T& List<T>::back()
	{
		return ( ( Node* ) _anchor._prev )->_data;
	}

	template<typename T>
	inline const T& List<T>::front() const
	{
		return ( ( Node* ) _anchor._next )->_data;
	}

	template<typename T>
	inline const T& List<T>::back() const
	{
		return ( ( Node* ) _anchor._prev )->_data;
	}


	template<typename T>
	inline List<T>& List<T>::operator=( const List<T>& list )
	{
		if( this != &list ) {
			clear();
			Node* it = list._anchor._next;
			while( it != &list._anchor ) {
				append( it->_data );
				it = it->_next;
			}
		}
		return *this;
	}

	template<typename T>
	inline void List<T>::append( const T& data )
	{
		if( _anchor._next == &_anchor ) {
			Node* node = new Node( data, &_anchor, &_anchor );
			_anchor._prev = node;
			_anchor._next = node;
		} else {
			Node* node = new Node( data, &_anchor, _anchor._prev );
			_anchor._prev->_next = node;
			_anchor._prev = node;
		}
		_size++;
	}

	template<typename T>
	inline void List<T>::prepend( const T& data )
	{
		if( _anchor._prev == &_anchor ) {
			Node* node = new Node( data, &_anchor, &_anchor );
			_anchor._next = node;
			_anchor._prev = node;
		} else {
			Node* node = new Node( data, _anchor._next, &_anchor );
			_anchor._next->_prev = node;
			_anchor._next = node;
		}
		_size++;
	}

	template<typename T>
	template<typename T2>
	inline typename List<T>::Iterator List<T>::find( const T2& data ) const
	{
		Node* it = ( Node* ) _anchor._next;
		while( it != &_anchor ) {
			if( it->_data == data )
				break;
			it = ( Node* ) it->_next;
		}
		return Iterator( it );
	}


	template<typename T>
	inline typename List<T>::Iterator List<T>::find( const T& data ) const
	{
		Node* it = ( Node* ) _anchor._next;
		while( it != &_anchor ) {
			if( it->_data == data )
				break;
			it = ( Node* ) it->_next;
		}
		return Iterator( it );
	}

	template<typename T>
	inline typename List<T>::Iterator List<T>::remove( Iterator it )
	{
		Iterator ret( it._it->_next );
		unlinkNode( it._it );
		_size--;
		delete ( Node* ) it._it;
		return ret;
	}

	template<typename T>
	inline typename List<T>::Iterator List<T>::remove( ReverseIterator it )
	{
		Iterator ret( it._it->_prev );
		unlinkNode( it._it );
		_size--;
		delete ( Node* ) it._it;
		return ret;
	}

	template<typename T>
	inline void List<T>::clear()
	{
		if( isEmpty() )
			return;

		Iterator it = begin();
		Iterator iend = end();
		while( it != iend ) {
			Iterator cur = it;
			it++;
			delete ( Node* ) cur._it;
		}
		_anchor._prev = &_anchor;
		_anchor._next = &_anchor;
		_size = 0;
	}

	template<typename T>
	inline void List<T>::unlinkNode( NodeBase* node )
	{
		node->_prev->_next = node->_next;
		node->_next->_prev = node->_prev;
	}

}

#endif
