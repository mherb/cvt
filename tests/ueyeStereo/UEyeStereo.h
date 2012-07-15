#ifndef CVT_UEYE_STEREO_H
#define	CVT_UEYE_STEREO_H

#include <cvt/util/String.h>
#include <cvt/util/Exception.h>
#include <cvt/io/UEyeUsbCamera.h>

namespace cvt
{
	/* convenience class for synced Ueye stereo */
	class UEyeStereo 
	{
		public:
			UEyeStereo( const String& masterId, const String& slaveId );
			~UEyeStereo();

			/**
			 *	\brief capture the next frame in both cameras
		   	 *	\return	true if a frame could be captured in both views
			 */
			bool nextFrame();
			
			const Image&  masterFrame() const { return _master->frame(); }	
			const Image&  slaveFrame()  const { return _slave->frame(); }	
			
			const String& masterId()    const { return _master->identifier(); }	
			const String& slaveId()     const { return _slave->identifier(); }	

			void setPixelClock( size_t mhz );

		private:
			UEyeUsbCamera*	_master;
			UEyeUsbCamera*	_slave;

			void syncCameraSettings();
			void setupSyncing();
	};

	inline UEyeStereo::UEyeStereo( const String& masterId, 
								   const String& slaveId ):
		_master( 0 ),
		_slave( 0 )
	{
		size_t numCams = UEyeUsbCamera::count();
		if( numCams < 2 ){
			throw CVTException( "Could not find two ueye cameras!" );
		}

		CameraInfo camInfo;
		for( size_t i = 0; i < numCams; i++ ){
			if( _master && _slave )
				break;

			UEyeUsbCamera::cameraInfo( i, camInfo );
			UEyeUsbCamera* tmp = new UEyeUsbCamera( i, 0 );  
			if( _master == 0 ){
				if( tmp->identifier() == masterId ){
					_master = tmp;
					continue;	
				}
			}
			if( _slave == 0 ){
				if( tmp->identifier() == slaveId ){
					_slave = tmp;
					continue;	
				}
			}
		}

		if( !_master )
			throw CVTException( "Could not find UEye Cam for master id" );
		if( !_slave )
			throw CVTException( "Could not find UEye Cam for slave id" );
	
		// we have master and slave camera!
		setPixelClock( 27 ); /* 27 should be best image quality for our ueyes*/

		syncCameraSettings();
		setupSyncing();
	}

	UEyeStereo::~UEyeStereo()
	{
		if( _master )
			delete _master;
		if( _slave )
			delete _slave;
	}

	inline void UEyeStereo::setPixelClock( size_t mhz )
	{
		_master->setPixelClock( mhz );
		_slave->setPixelClock( mhz );
	}

	inline bool UEyeStereo::nextFrame()
	{
		// TODO: next frame should return if it was successful
		_master->nextFrame();
		_slave->nextFrame();

		return true;
	}

	inline void UEyeStereo::syncCameraSettings()
	{
		// same gains:
		{
			int m, r, g, b;
			_master->getHardwareGains( m, r, g, b );
			_slave->setHardwareGains( m, r, g, b );
		}

		{
			_master->saveParameters( "tmp.ini" );
			_slave->loadParameters( "tmp.ini" );
		}
	}

	inline void UEyeStereo::setupSyncing()
	{
		// master is triggered by software!
		_master->setRunMode( UEyeUsbCamera::UEYE_MODE_TRIGGERED );
		// slave is triggered by hardware (master)!
		_slave->setRunMode( UEyeUsbCamera::UEYE_MODE_HW_TRIGGER );
		_master->setFlashMode( UEyeUsbCamera::FLASH_LOW_ON_EXPOSURE );
		_slave->setTriggerMode( UEyeUsbCamera::TRIGGER_HI_LO );
		_master->setFlashDelayAndDuration( 0, 20 );
		_slave->setTriggerDelay( 0 );
	}

}

#endif
