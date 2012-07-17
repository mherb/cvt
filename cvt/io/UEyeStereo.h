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

            void setHorizontalMirror( bool value );
            void setVerticalMirror( bool value );
            void setAutoShutter( bool value );
            void setAutoSensorShutter( bool value );
            void setFramerate( double value );
            void setExposureTime( double value );

            const UEyeUsbCamera& master() const { return *_master; }
            const UEyeUsbCamera& slave() const { return *_slave; }

		private:
			UEyeUsbCamera*	_master;
			UEyeUsbCamera*	_slave;

            size_t _nM, _nS;

			void syncCameraSettings();
			void setupSyncing();
	};

	inline UEyeStereo::UEyeStereo( const String& masterId, 
								   const String& slaveId ):
		_master( 0 ),
        _slave( 0 ),
        _nM( 0 ), _nS( 0 )
	{
		size_t numCams = UEyeUsbCamera::count();
		if( numCams < 2 ){
			throw CVTException( "Could not find two ueye cameras!" );
		}

		for( size_t i = 0; i < numCams; i++ ){
			if( _master && _slave )
				break;

            CameraInfo camInfo;
			UEyeUsbCamera::cameraInfo( i, camInfo );            
            UEyeUsbCamera* tmp = new UEyeUsbCamera( camInfo.index(), camInfo.mode( 0 ) );

			if( _master == 0 ){
				if( tmp->identifier() == masterId ){
                    std::cout << "Master FOUND" << std::endl;
					_master = tmp;
					continue;	
				}
			}
			if( _slave == 0 ){
				if( tmp->identifier() == slaveId ){
                    std::cout << "Slave FOUND" << std::endl;
					_slave = tmp;
					continue;	
				}
			}

            delete tmp;
		}

		if( !_master )
			throw CVTException( "Could not find UEye Cam for master id" );
		if( !_slave )
			throw CVTException( "Could not find UEye Cam for slave id" );
			
        syncCameraSettings();
        setupSyncing();

        _slave->startCapture();
        _master->startCapture();

        _slave->nextFrame( 0 );
        _master->nextFrame( 0 );
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

    inline void UEyeStereo::setHorizontalMirror( bool value )
    {
        _master->setHorizontalMirror( value );
        _slave->setHorizontalMirror( value );
    }

    inline void UEyeStereo::setVerticalMirror( bool value )
    {
        _master->setVerticalMirror( value );
        _slave->setVerticalMirror( value );
    }

    inline void UEyeStereo::setAutoShutter( bool value )
    {
        _master->setAutoShutter( value );
        _slave->setAutoShutter( value );
    }

    inline void UEyeStereo::setAutoSensorShutter( bool value )
    {
        _master->setAutoSensorShutter( value );
        _slave->setAutoSensorShutter( value );
    }

    inline void UEyeStereo::setFramerate( double value )
    {
        _master->setFramerate( value );
        _slave->setFramerate( value );
    }

    inline void UEyeStereo::setExposureTime( double value )
    {
        _master->setExposureTime( value );
        _slave->setExposureTime( value );
    }


	inline bool UEyeStereo::nextFrame()
    {        
        bool gotM = _master->waitFrame( 500 );
        bool gotS = _slave->waitFrame( 500 );

        if( gotM ) _nM++;
        if( gotS ) _nS++;

        if( _nM != _nS )
            std::cout << "Capture Difference: " << _nM - _nS << std::endl;

        _slave->nextFrame( 0 );
        _master->nextFrame( 0 );

        return ( gotM & gotS );
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
            //_master->saveParameters( "tmp.ini" );
            //_slave->loadParameters( "tmp.ini" );
		}
	}

	inline void UEyeStereo::setupSyncing()
	{
		// master is triggered by software!
        _master->setRunMode( UEyeUsbCamera::UEYE_MODE_TRIGGERED );
        _master->setFlashMode( UEyeUsbCamera::FLASH_LOW_ON_EXPOSURE );

        //_master->setRunMode( UEyeUsbCamera::UEYE_MODE_FREERUN );
        //_master->setFlashMode( UEyeUsbCamera::FLASH_LOW_ON_EXPOSURE_FR );

        _master->setFlashDelayAndDuration( 200, 200 );

		// slave is triggered by hardware (master)!
		_slave->setRunMode( UEyeUsbCamera::UEYE_MODE_HW_TRIGGER );
        _slave->setTriggerMode( UEyeUsbCamera::TRIGGER_HI_LO );
        _slave->setTriggerDelay( 0 );
	}

}

#endif
