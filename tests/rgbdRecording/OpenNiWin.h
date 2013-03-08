#ifndef CVT_OPENNI_WIN_H
#define CVT_OPENNI_WIN_H

#include <cvt/gui/TimeoutHandler.h>
#include <cvt/io/OpenNICamera.h>
#include <cvt/io/RawVideoWriter.h>
#include <cvt/io/FileSystem.h>
#include <cvt/util/Time.h>
#include <cvt/util/Thread.h>
#include <cvt/util/TQueue.h>
#include <cvt/util/ConfigFile.h>

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
				_stop( false ),
				_rgbWriter( "rgb.cvtraw" ),
				_depthWriter( "depth.cvtraw" )
			{
//				if( !FileSystem::exists( folder ) ){
//					FileSystem::mkdir( folder );
//				}
			}

			~ImageWriter()
			{
				_stop = true;
				this->join();
			}

			void execute( void* )
			{

				//Image dFloat;
				while( !_stop ){
					QueueData* d = _queue.waitNext();

//					String out;
//					out.sprintf( "%s/rgb_%07d.cvtraw", _folder.c_str(), _iter );
//					d->_rgb.save( out );
//					out.sprintf( "%s/depth_%07d.cvtraw", _folder.c_str(), _iter );
//					d->_depth.save( out );
//					_iter++;

					//d->_depth.convert( dFloat, IFormat::GRAY_FLOAT );
					_rgbWriter.write( d->_rgb );
					_depthWriter.write( d->_depth );

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

			RawVideoWriter		_rgbWriter;
			RawVideoWriter		_depthWriter;

	};

	class OpenNiRecorder
	{
		public:
			OpenNiRecorder( ConfigFile& cfg );
			~OpenNiRecorder();

			void run();

		private:
			OpenNICamera	_cam;
			ImageWriter		_writer;
			Image			_dFloat;
	};

	inline OpenNiRecorder::OpenNiRecorder( ConfigFile& cfg ) :
		_cam( 0, CameraMode( cfg.valueForName<int>( "width", 640 ),
							 cfg.valueForName<int>( "height", 480 ),
							 cfg.valueForName<int>( "fps", 30 ),
							 IFormat::UYVY_UINT8 ) ),
		_writer( "rgbdRecording" )
	{
		_writer.run( NULL );

        _cam.startCapture();
        _cam.setSyncRGBDepth( cfg.valueForName<bool>( "sync_frames", false ) );
        _cam.setRegisterDepthToRGB( cfg.valueForName<bool>( "register_frames", true ) );
        cfg.save( "config.cfg" );
	}

	inline OpenNiRecorder::~OpenNiRecorder()
	{
		_cam.stopCapture();
	}

	inline void OpenNiRecorder::run()
	{
		while( true ){
			_cam.nextFrame();
			ImageWriter::QueueData* data = new ImageWriter::QueueData( _cam.frame(), _cam.depth() );
			_writer.addData( data );
		}
	}
}

#endif
