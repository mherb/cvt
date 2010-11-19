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

- (void)captureOutput:(QTCaptureOutput *)captureOutput 
  didOutputVideoFrame:(CVImageBufferRef)videoFrame 
	 withSampleBuffer:(QTSampleBuffer *)sampleBuffer 
	   fromConnection:(QTCaptureConnection *)connection
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

- (void)captureOutput:(QTCaptureOutput *)captureOutput 
didDropVideoFrameWithSampleBuffer:(QTSampleBuffer *)sampleBuffer 
	   fromConnection:(QTCaptureConnection *)connection 
{
	//std::cout << "Camera dropped frame!" << std::endl; 
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
	
	size_t width = CVPixelBufferGetWidth( data );
	size_t height = CVPixelBufferGetHeight( data );
	size_t rowBytes = CVPixelBufferGetBytesPerRow( data );
	
	if ( rowBytes != 0 ) {
		// test if image sizes match
		if ( width != img->width() ||
			 height != img->height() ||
			 img->format() != cvt::IFormat::RGBA_UINT8 ) {
			 img->reallocate( width, height, cvt::IFormat::RGBA_UINT8 );
		}		
				
		// copy the image: 
		size_t stride;
		uint8_t * dstBase = img->map( &stride );
		if( rowBytes == stride ){
			memcpy( dstBase, sourceBase, height*stride );
		} else {
			size_t i = height;
			cvt::SIMD * simd = cvt::SIMD::get();
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
			static void cameraInfo( size_t index, CameraInfo & info );
		
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
											    size_t fps, 
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
		
		BOOL success = [dev open:nil];
		if( !success ){
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
		
		NSDictionary * attr = [NSDictionary dictionaryWithObjectsAndKeys:
							   [NSNumber numberWithInt:width], kCVPixelBufferWidthKey,
							   [NSNumber numberWithInt:height], kCVPixelBufferHeightKey,
							   [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA], (id)kCVPixelBufferPixelFormatTypeKey,
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
		std::cout << "Starting session" << std::endl;
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
	
	void QTKitCameraInterface::cameraInfo( size_t index, CameraInfo & info )
	{
		
	}

	QTKitCamera::QTKitCamera( size_t camIndex, 
							  size_t width, 
							  size_t height, 
							  size_t fps, 
							  const IFormat & format ):
		_frame( width, height, format )
	{
		_device = new QTKitCameraInterface( camIndex, width, height, fps, format );
	}
	
	QTKitCamera::~QTKitCamera()
	{
		delete _device;
	}
	
	void QTKitCamera::nextFrame()
	{
		_device->nextFrame( _frame );
	}
	
	void QTKitCamera::startCapture()
	{	
		_device->startCapture();
	}
	
	void QTKitCamera::stopCapture()
	{
		_device->stopCapture();
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
	}
	
}