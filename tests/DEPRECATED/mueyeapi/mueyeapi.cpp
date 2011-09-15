#include <ueye.h>
#include <iostream>
#include <string.h>


size_t cameraCount()
{
	UEYE_CAMERA_INFO camInfo;
	ZeroMemory( &camInfo, sizeof( UEYE_CAMERA_INFO ) );

    UEYE_CAMERA_LIST * camList = new UEYE_CAMERA_LIST;
    camList->dwCount = 0;

    if( is_GetCameraList( camList ) == IS_SUCCESS ) {
        DWORD dw = camList->dwCount;
        delete camList;

        // Reallocate the required camera list size
        camList = ( PUEYE_CAMERA_LIST )new char[ sizeof( DWORD ) + dw * sizeof( UEYE_CAMERA_INFO ) ];
        camList->dwCount = dw;

        // Get CameraList and store it ...
        if( is_GetCameraList( camList ) == IS_SUCCESS ){
			for( int i = 0; i < ( int )dw; i++ ){
				std::cout << "Camera " << i <<
					"\n\tcamId\t" << camList->uci[ i ].dwCameraID <<
					"\n\tdevId\t" << camList->uci[ i ].dwDeviceID <<
					"\n\tsensorId\t" << camList->uci[ i ].dwSensorID <<
					"\n\tinUse?\t" << camList->uci[ i ].dwInUse <<
					"\n\tSerial\t" << camList->uci[ i ].SerNo << 
					"\n\tModel\t" << camList->uci[ i ].Model << std::endl;
			}
        }
	}

	return camList->dwCount;
}

int main()
{
	for( size_t i = 0; i < 10; i++ )
	{
		std::cout << cameraCount() << std::endl;
	}

	return 0;
}
