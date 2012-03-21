#ifndef CVT_OPENNI_WIN_H
#define CVT_OPENNI_WIN_H

#include <cvt/gui/TimeoutHandler.h>
#include <cvt/io/OpenNICamera.h>
#include <cvt/io/RawVideoWriter.h>
#include <cvt/io/FileSystem.h>
#include <cvt/util/Time.h>
#include <cvt/util/Thread.h>
#include <cvt/util/TQueue.h>

namespace cvt
{
	class ImageWriter : public Thread<void>
	{
		public:
			struct QueueData
			{
				QueueData( const Image& rgb, const Image& depth ) :
					_rgb( rgb ), _depth( depth )
				{
				}

				Image _rgb;
				Image _depth;
			};

			ImageWriter( const String& folder ) :
				_folder( folder ),
				_iter( 0 ),
				_stop( false )
			{
				if( !FileSystem::exists( folder ) ){
					FileSystem::mkdir( folder );
				}
			}

			~ImageWriter()
			{
				_stop = true;
				this->join();
			}

			void execute( void* )
			{
				while( !_stop ){
					QueueData* d = _queue.waitNext();

					String out;
					out.sprintf( "%s/rgb_%07d.cvtraw", _folder.c_str(), _iter );
					d->_rgb.save( out );
					out.sprintf( "%s/depth_%07d.cvtraw", _folder.c_str(), _iter );
					d->_depth.save( out );
					_iter++;

					if( ( _iter % 150 ) == 0 ){
						std::cout << "#";
						std::flush( std::cout );
					}

					delete d;
				}
			}

			void addData( QueueData* d )
			{
				_queue.enqueue( d );
			}

		private:
			String				_folder;
			size_t				_iter;
			bool				_stop;
			TQueue<QueueData*>	_queue;

	};

	class OpenNiWin : public TimeoutHandler
	{
		public:
			OpenNiWin();
			~OpenNiWin();

			void onTimeout();

		private:
			uint32_t		_timerId;
			OpenNICamera	_cam;
			ImageWriter		_writer;
	};

	inline OpenNiWin::OpenNiWin() :
		_cam( 0, CameraMode( 640, 480, 30, IFormat::UYVY_UINT8 ) ),
		_writer( "rgbdRecording" )
	{
		_writer.run( NULL );

		_timerId = Application::registerTimer( 10, this );
        _cam.startCapture();
	}

	inline OpenNiWin::~OpenNiWin()
	{
		Application::unregisterTimer( _timerId );
		_cam.stopCapture();
	}

	inline void OpenNiWin::onTimeout()
	{
		_cam.nextFrame();

		ImageWriter::QueueData* data = new ImageWriter::QueueData( _cam.frame(), _cam.depth() );
		_writer.addData( data );
	}
}

#endif
