#include "muEyeUsbCamera.h"

#include <iostream>

namespace cvt
{
    int muEyeUsbCamera::availableCameras()
    {
        int ret;

        is_GetNumberOfCameras(&ret);

        return ret;
    }

    muEyeUsbCamera::muEyeUsbCamera(unsigned int camIndex,
                                 CameraParameters const& params):
        AbstractCamera(params),
        mCamIndex(camIndex)
    {
    }

    muEyeUsbCamera::~muEyeUsbCamera()
    {
        if ( m_hCam != 0 )
        {
            //free old image mem.
            is_FreeImageMem( m_hCam, m_pcImageMemory, m_lMemoryId );
            is_ExitCamera( m_hCam );
        }

        cvReleaseImage(&img);
    }

    void muEyeUsbCamera::setAutoGain(bool value)
    {
        double pVal1 = value?1:0;
        double pVal2 = 0;
        is_SetAutoParameter(m_hCam, IS_SET_ENABLE_AUTO_GAIN, &pVal1, &pVal2);
    }

    void muEyeUsbCamera::setAutoShutter(bool value)
    {
        double pVal1 = value?1:0;
        double pVal2 = 0;
        is_SetAutoParameter(m_hCam, IS_SET_ENABLE_AUTO_SHUTTER, &pVal1, &pVal2);
    }

    void muEyeUsbCamera::setAutoSensorShutter(bool value)
    {
        double pVal1 = value?1:0;
        double pVal2 = 0;
        is_SetAutoParameter(m_hCam, IS_SET_ENABLE_AUTO_SENSOR_SHUTTER, &pVal1, &pVal2);
    }

    void muEyeUsbCamera::setAutoWhiteBalance(bool value)
    {
        double pVal1 = value?1:0;
        double pVal2 = 0;
        is_SetAutoParameter(m_hCam, IS_SET_ENABLE_AUTO_WHITEBALANCE, &pVal1, &pVal2);
    }

    void muEyeUsbCamera::setMaxAutoShutter(double value)
    {
        double pVal1 = value;
        double pVal2 = 0;
        is_SetAutoParameter(m_hCam, IS_SET_AUTO_SHUTTER_MAX, &pVal1, &pVal2);
    }

    void muEyeUsbCamera::setExposureTime(double value)
    {
        double pVal1 = value;
        is_SetExposureTime (m_hCam, pVal1, &pVal1);
    }

    void muEyeUsbCamera::setPixelClock(unsigned int value)
    {
        is_SetPixelClock (m_hCam, value);
    }

    void muEyeUsbCamera::setHorizontalMirror(bool value)
    {
        is_SetRopEffect (m_hCam, IS_SET_ROP_MIRROR_UPDOWN, value?1:0, 0);
    }

    void muEyeUsbCamera::setVerticalMirror(bool value)
    {
        is_SetRopEffect (m_hCam, IS_SET_ROP_MIRROR_LEFTRIGHT, value?1:0, 0);
    }

    void muEyeUsbCamera::setParameters(CameraParameters const& params)
    {
        this->setAutoGain(params.autoGain);
        this->setAutoShutter(params.autoShutter);
        this->setAutoSensorShutter(params.autoSensorShutter);
        this->setAutoWhiteBalance(params.autoWhiteBalance);
        this->setMaxAutoShutter(params.maxAutoShutter);
        this->setExposureTime(params.exposureTime);
        this->setPixelClock(params.pixelClock);
    }

    void muEyeUsbCamera::initCam()
    {
        nRet = is_InitCamera (&m_hCam, NULL);
        /************************************************************************************************/
        /*                                                                                              */
        /*  If the camera returns with "IS_STARTER_FW_UPLOAD_NEEDED", an upload of a new firmware       */
        /*  is necessary. This upload can take several seconds. We recommend to check the required      */
        /*  time with the function is_GetDuration().                                                    */
        /*                                                                                              */
        /*  In this case, the camera can only be opened if the flag "IS_ALLOW_STARTER_FW_UPLOAD"        */
        /*  is "OR"-ed to m_hCam. This flag allows an automatic upload of the firmware.                 */
        /*                                                                                              */
        /************************************************************************************************/
        if (nRet == IS_STARTER_FW_UPLOAD_NEEDED)
        {
            // Time for the firmware upload = 25 seconds by default
            INT nUploadTime = 25000;
            is_GetDuration (m_hCam, IS_SE_STARTER_FW_UPLOAD, &nUploadTime);


            std::cout << "This camera requires a new firmware. The upload will take around" << nUploadTime / 1000<<" "<<"seconds. Please wait .."<<std::endl;

            // Try again to open the camera. This time we allow the automatic upload of the firmware by
            // specifying "IS_ALLOW_STARTER_FIRMWARE_UPLOAD"
            m_hCam = (HIDS) (((INT)m_hCam) | IS_ALLOW_STARTER_FW_UPLOAD);
            nRet = is_InitCamera (&m_hCam, NULL);
        }

    }


    void muEyeUsbCamera::open(CameraParameters const& params)
    {
        if ( m_hCam != 0 ) {
            //free old image mem.
            is_FreeImageMem( m_hCam, m_pcImageMemory, m_lMemoryId );
            is_ExitCamera( m_hCam );
        }

        int nCameras = muEyeUsbCamera::availableCameras();
        std::cout<< "Number of Cameras " << nCameras << std::endl;

        // IS internal index starts by one
        m_hCam = mCamIndex + 1;

        int camId;

        //Initialize the camera
        initCam();

        this->setParameters(params);

        if(nRet == IS_SUCCESS){
            // retrieve original image size
            SENSORINFO sInfo;

            is_GetSensorInfo( m_hCam, &sInfo );

            // display initialization
            INT xPos;
            INT yPos;
            xPos = 0;
            yPos = 0;;
            m_nSizeX = params.width;
            m_nSizeY = params.height;

            double newFPS;

            //is_SetImageSize( m_hCam, m_nSizeX, m_nSizeY );
            is_SetAOI( m_hCam, IS_SET_IMAGE_AOI, &xPos, &yPos, &m_nSizeX, &m_nSizeY);

            std::cout<<"Image Properties m_nSizeX m_nSizeY "<<m_nSizeX<<" "<<m_nSizeY<<std::endl;

            if(is_SetFrameRate (m_hCam, params.fps, &newFPS) == IS_SUCCESS){
                std::cout<<"Frame Rate set "<< newFPS <<std::endl;
            } else{
                std::cout<<"Could not set FrameRate"<<std::endl;
            }

            this->setHorizontalMirror(params.horizonticalMirror);
            this->setHorizontalMirror(params.verticalMirror);

            // memory initialization
            is_AllocImageMem(	m_hCam,
                             m_nSizeX,
                             m_nSizeY,
                             24,
                             &m_pcImageMemory,
                             &m_lMemoryId);

            is_SetImageMem( m_hCam, m_pcImageMemory, m_lMemoryId );	// set memory active

            is_SetColorMode(m_hCam, IS_CM_BGR8_PACKED);

            img = cvCreateImage(cvSize(m_nSizeX, m_nSizeY), 8, 3);

            _width = m_nSizeX;
            _height = m_nSizeY;

            // enable the dialog based error report
            m_Ret = is_SetErrorReport(m_hCam, IS_ENABLE_ERR_REP); //IS_DISABLE_ERR_REP);
            if( m_Ret != IS_SUCCESS )
            {
                std::cout<<"ERROR: Can not enable the automatic uEye error report!"<<std::endl;
            }

#ifdef _WIN32_WCE
            //LoadParameters();
#endif

        } else {
            std::cout<<"ERROR: Can not open uEye camera!"<<std::endl;
        }
    }

    void muEyeUsbCamera::captureStart()
    {
        is_CaptureVideo (m_hCam, IS_WAIT);
    }

    void muEyeUsbCamera::captureStop()
    {
    }

    void muEyeUsbCamera::captureNext()
    {
        //is_FreezeVideo( m_hCam, IS_WAIT );

        //	if ( m_hCam != 0 ){
        //		is_FreezeVideo( m_hCam, IS_WAIT );
        //	} else {
        //		std::cout<<"Error: Camera not initialized!"<<std::endl;
        //	}
        is_CopyImageMem(m_hCam, m_pcImageMemory, m_lMemoryId, img->imageData);
    }

    IplImage const* muEyeUsbCamera::image()
    {
        //double pVal1, pVal2;

        //double dblFPS;
        //is_GetFramesPerSecond (m_hCam, &dblFPS);
        //std::cout<<"FrameRate "<<dblFPS<<std::endl;

        return img;
    }

}
