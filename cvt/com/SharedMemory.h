#ifndef CVT_SHAREDMEMORY_H
#define CVT_SHAREDMEMORY_H

#include <string>

#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>

#include <cvt/util/String.h>
#include <cvt/io/FileSystem.h>

namespace cvt
{
	template <class T>
	class SharedMemory
	{
		public:
			SharedMemory( int32_t id );
			SharedMemory( const cvt::String& name );
			~SharedMemory();

			const T& data() const { return _ptr->data; }

			// internal data is locked and unlocked automatically
			void set( const T& data );
			
			// user needs to lock the data externally!
			T& data() { return _ptr->data; }

			/* external locking and unlocking of data */
			void lock();
			void unlock();
		private:
			
			struct SharedData
			{
				sem_t	mutex;
				T		data;
			};

			int			_id;
			SharedData*	_ptr;

			key_t idForName( const String& name ) const;
			void init( key_t id );
	};

	template <class T>
	inline SharedMemory<T>::SharedMemory( int32_t id ) :
		_id( -1 ),
		_ptr( 0 )
	{
		init( id );
	}

	template <class T>
	inline SharedMemory<T>::SharedMemory( const cvt::String& name ) :
		_id( -1 ),
		_ptr( 0 )
	{
		key_t id = idForName( name );
		init( id );
	}

	template <class T>
	inline key_t SharedMemory<T>::idForName( const String& name ) const
	{
		cvt::String path;
		path.sprintf( "/tmp/%s", name.c_str() );
		if( !FileSystem::exists( path ) ){
			FileSystem::touch( path );
		}
		return ftok( path.c_str(), 0 );
	}

	template <class T>
	void SharedMemory<T>::init( int32_t id )
	{
		// always create the segment, if it's not available yet
		_id = shmget( ( key_t )id, sizeof( SharedData ), IPC_CREAT | S_IRWXU | S_IRWXG );

		if( _id == -1 ){
			// error throw exception:
			perror( "shmget" );
			throw CVTException( "Could not create SharedMemory file" );
		}

		_ptr = ( SharedData* )shmat( _id, NULL, 0 );

		if( _ptr == (void*)-1 ){
			perror( "shmat" );
			throw CVTException( "Could not open SharedMemory file" );
		}

#ifndef APPLE
		if( sem_init( &( _ptr->mutex ), 1, 1 ) != 0 ){
			perror( "sem_init" );
			throw CVTException( "Could not initialize semaphore" );
		}
#endif
	}

	template <class T>
	inline SharedMemory<T>::~SharedMemory()
	{
		if( shmdt( (void*)_ptr ) == -1 ){
			perror( "shmdt" );
			throw CVTException( "Could not detach shared memory" );
		}

		/* remove the segment if this was the last connected process */
		struct shmid_ds shm_info;
		shmctl( _id, IPC_STAT , &shm_info );
		if( shm_info.shm_nattch == 0 ){
			shmctl( _id, IPC_RMID, 0 );
		}

	}

	template <class T>
	inline void SharedMemory<T>::lock()
	{
		sem_wait( &_ptr->mutex );
	}

	template <class T>
	inline void SharedMemory<T>::unlock()
	{
		sem_post( &_ptr->mutex );
	}

	template <class T>
	inline void SharedMemory<T>::set( const T & data )
	{
		lock();
		_ptr->data = data;
		unlock();
	}
}

#endif
