#include <cvt/io/QTKitCamera.h>
#include <cvt/util/Exception.h>
#include <cvt/util/SIMD.h>

#import <QTKit/QTKit.h>


@interface CameraDelegate : NSObject
{
	CVImageBufferRef currentFrame;
	bool isNew;
}

- (void)captureOutput:(QTCaptureOutput *)captureOutput 
  didOutputVideoFrame:(CVImageBufferRef)videoFrame 
	 withSampleBuffer:(QTSampleBuffer *)sampleBuffer 
	   fromConnection:(QTCaptureConnection *)connection;

- (void)captureOutput:(QTCaptureOutput *)captureOutput 
didDropVideoFrameWithSampleBuffer:(QTSampleBuffer *)sampleBuffer 
	   fromConnection:(QTCaptureConnection *)connection;

-(void) copyCurrentFrame:(cvt::Image*) img;
-(bool) isNewFrame;

@end

@implementation CameraDelegate

-(id) init
{
	self = [ super init ];
	if( self ){
		isNew = false;
	}
	return self;
}

-(void) dealloc
{
	CVBufferRelease( currentFrame );
	[super dealloc];
}

- (void)captureOutput:( QTCaptureOutput * ) __attribute__((unused))	captureOutput
  didOutputVideoFrame:( CVImageBufferRef  )		videoFrame
	 withSampleBuffer:( QTSampleBuffer * ) __attribute__((unused)) sampleBuffer
	   fromConnection:( QTCaptureConnection * ) __attribute__((unused)) connection
{
	CVImageBufferRef imageBufferToRelease;

    CVBufferRetain(videoFrame);

    @synchronized (self) {
        imageBufferToRelease = currentFrame;
        currentFrame = videoFrame;
		isNew = true;
    }

    CVBufferRelease(imageBufferToRelease);
}

- (void)captureOutput:(QTCaptureOutput *) __attribute__((unused)) captureOutput
didDropVideoFrameWithSampleBuffer:(QTSampleBuffer *) __attribute__((unused)) sampleBuffer
	   fromConnection:(QTCaptureConnection *) __attribute__((unused)) connection
{
}


-(void) copyCurrentFrame:(cvt::Image*) img
{
	CVPixelBufferRef data; 	
	@synchronized (self){
		data = CVBufferRetain( currentFrame );
		isNew = false; 
	}
	
	CVPixelBufferLockBaseAddress( data, 0 );
	uint8_t* sourceBase = ( uint8_t* )CVPixelBufferGetBaseAddress( data );
	
	size_t height = CVPixelBufferGetHeight( data );
	size_t rowBytes = CVPixelBufferGetBytesPerRow( data );
	
	if ( rowBytes != 0 ) {						
		// copy the image: 
		size_t stride;
		uint8_t * dstBase = img->map( &stride );
		if( rowBytes == stride ){
			memcpy( dstBase, sourceBase, height*stride );
		} else {
			size_t i = height;
			cvt::SIMD * simd = cvt::SIMD::instance();
			uint8_t * dst = dstBase;			
			while( i-- ){
				simd->Memcpy( dst, sourceBase, rowBytes );
				dstBase += stride;
				sourceBase += rowBytes;
			}
		}
		
		img->unmap( dstBase );
	}
	
	CVPixelBufferUnlockBaseAddress( data, 0 );
	CVBufferRelease( data ); 
}

-(bool) isNewFrame
{
	bool ret;
	@synchronized(self){
		ret = isNew;
	}
	return ret;
}

@end


namespace cvt {
	
	class QTKitCameraInterface
	{
		public:
			QTKitCameraInterface( size_t camIndex, 
								  size_t width, 
								  size_t height, 
								  size_t fps, 
								  const IFormat & format );
		
			~QTKitCameraInterface();	
			
			void nextFrame( cvt::Image & img );
			void startCapture();
			void stopCapture();

			static size_t count();
		
		private:	
			NSAutoreleasePool * _pool;
			QTCaptureDeviceInput * _qtDevice;
			QTCaptureSession * _session;
			CameraDelegate * _camDelegate;
			QTCaptureDecompressedVideoOutput  * _output;
	};
	
	QTKitCameraInterface::QTKitCameraInterface( size_t camIndex,
											    size_t width, 
											    size_t height, 
											    size_t , 
											    const IFormat & format ):
		_pool( 0 ),
		_qtDevice( 0 ),
		_session( 0 ),
		_camDelegate( 0 ),
		_output( 0 )
	{
		_pool = [[ NSAutoreleasePool alloc ] init];
		NSArray * availableDevices = [ QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo ];
					
		QTCaptureDevice * dev = [ availableDevices objectAtIndex:camIndex ];
		
		std::cout << "Device name: " << [[dev localizedDisplayName] UTF8String] << std::endl;
		
		NSError * error;
		BOOL success = [dev open:&error];
				
		if( success == NO ){
			std::cout << "Error: " << [[error localizedDescription] UTF8String] << std::endl;			
			throw CVTException( "Could not open device" );
		}
		
		_qtDevice = [[QTCaptureDeviceInput alloc] initWithDevice:dev];
		_session = [[QTCaptureSession alloc] init];
		
		success = [_session addInput:_qtDevice error:nil];		
		if( !success ){
			throw CVTException( "Could not add input device to session" );
		}
	
		_output = [[QTCaptureDecompressedVideoOutput alloc] init];
		_camDelegate = [ [ CameraDelegate alloc ] init ];
		[_output setDelegate:_camDelegate];
		[_output setAutomaticallyDropsLateVideoFrames:YES];
		
		NSNumber* pixFormat;
		if( format == IFormat::BGRA_UINT8 )
			pixFormat = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA];
		else if( format == IFormat::RGBA_UINT8 ){
			pixFormat = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32RGBA];
		} else {
			throw CVTException( "No matching pixel format in QTKit" );
		}
		
		NSDictionary * attr = [NSDictionary dictionaryWithObjectsAndKeys:
							   [NSNumber numberWithInt:width], kCVPixelBufferWidthKey,
							   [NSNumber numberWithInt:height], kCVPixelBufferHeightKey,
							   pixFormat, (id)kCVPixelBufferPixelFormatTypeKey,
							   nil];
		[_output setPixelBufferAttributes:attr];

		success = [_session addOutput:_output error:nil];		
		if( !success ){
			throw CVTException( "Could not add output to session" );
		}		
	}
	
	QTKitCameraInterface::~QTKitCameraInterface()
	{
        
		if( _qtDevice )
			[_qtDevice release];
		if( _camDelegate )
			[_camDelegate release];
		if( _session ){
			this->stopCapture();
			[_session release];
		}			
		if( _output )
			[_output release];
		if( _pool )
			[_pool release];
	}
	
	void QTKitCameraInterface::nextFrame( cvt::Image & img )
	{		
		double sleepTime = 0.01; 
		NSDate *loopUntil = [NSDate dateWithTimeIntervalSinceNow:sleepTime];
		
		while( ![ _camDelegate isNewFrame ] && 
			   [ [ NSRunLoop currentRunLoop ] runMode: NSDefaultRunLoopMode beforeDate:loopUntil ] ){
			loopUntil = [NSDate dateWithTimeIntervalSinceNow:sleepTime];
		}
		
		[ _camDelegate copyCurrentFrame: &img ];
	}
	
	void QTKitCameraInterface::startCapture()
	{   
        [_session startRunning];         
	}
	
	void QTKitCameraInterface::stopCapture()
	{
        [_session stopRunning];
	}
	
	
	size_t QTKitCameraInterface::count()
	{			
		NSAutoreleasePool * pool = [[ NSAutoreleasePool alloc ] init];
		size_t count = [[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo] count ];
		[pool release];
		
		return count;
	}
	
	QTKitCamera::QTKitCamera( size_t camIndex, const CameraMode & mode):
		_device( NULL ),
		_frame( mode.width, mode.height, mode.format ),
        _capturing( false )
	{
		_device = new QTKitCameraInterface( camIndex, mode.width, mode.height, mode.fps, mode.format );
	}
	
	QTKitCamera::~QTKitCamera()
	{
        if( _capturing )
            stopCapture();
		delete _device;
	}
	
	void QTKitCamera::nextFrame()
	{
		_device->nextFrame( _frame );
	}
	
	void QTKitCamera::startCapture()
	{	
        if( !_capturing ){
            _device->startCapture();
            _capturing = true;
        }
	}
	
	void QTKitCamera::stopCapture()
	{
        if( _capturing ){
            _device->stopCapture();
            _capturing = false;
        }
	}
	
	size_t QTKitCamera::count()
	{	
		return QTKitCameraInterface::count();
	}
	
	void QTKitCamera::cameraInfo( size_t index, CameraInfo & info )
	{
		NSAutoreleasePool * pool = [[ NSAutoreleasePool alloc ] init];
		NSArray * availableDevices = [ QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo ];
		
		if( index > [ availableDevices count] )
			throw CVTException( "Camera index out of bounds!" );
		
		QTCaptureDevice * dev = [ availableDevices objectAtIndex:index ];
		
		info.setName( [[dev localizedDisplayName] UTF8String] );
		info.setIndex( index );
		info.setType( CAMERATYPE_QTKIT );
		
		// FIXME: how can we find out the supported modes from QTKit? Is scales to whatever you want ...
		CameraMode mode( 640, 480, 30, IFormat::BGRA_UINT8 );
		info.addMode( mode );
		
		[pool release];
	}
	
}
