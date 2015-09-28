#include <cstdio>
#include <cassert>

#include "geomagic/geomagic.h"

using namespace std;

/* Static prototypes */
HDCallbackCode HDCALLBACK updateDeviceCallback(void *pUserData);
HDCallbackCode HDCALLBACK copyDeviceDataCallback(void *pUserData);


GeoMagic::GeoMagic(void)
{
    HDErrorInfo error;

    /* Start values */
    hUpdateHandle = 0;


    /* Initialize the device, must be done before attempting to call any hd 
       functions. */
    hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize the device");
        active = false;
        return;
    }

    /* Schedule the main scheduler callback that updates the device state. */
    hUpdateHandle = hdScheduleAsynchronous(updateDeviceCallback,
                                           &deviceData,
                                           HD_MAX_SCHEDULER_PRIORITY);

    /* Start the servo loop scheduler. */
    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start the scheduler");
        active = false;
        return;
    }

    active = true;
}


GeoMagic::~GeoMagic(void)
{
    /* For cleanup, unschedule callbacks and stop the servo loop. */
    hdStopScheduler();
    hdUnschedule(hUpdateHandle);
    hdDisableDevice(hHD);
}


bool
GeoMagic::isActive()
{
    return active;
}


void
GeoMagic::getDeviceData(double *pos, double *rpy, int *b1, int *b2)
{
    /* Instantiate the structure used to capture data from the device. */
    DeviceData destDeviceData;
    DeviceSourceDestPointers pSourceDestPointers;
    pSourceDestPointers.source = &deviceData;
    pSourceDestPointers.dest = &destDeviceData;

    /* Perform a synchronous call to copy the most current device state.
       This synchronous scheduler call ensures that the device state
       is obtained in a thread-safe manner. */
    hdScheduleSynchronous(copyDeviceDataCallback,
                          &pSourceDestPointers,
                          HD_MIN_SCHEDULER_PRIORITY);

    /* Get values. */
    *b1    = destDeviceData.m_button1State;
    *b2    = destDeviceData.m_button2State;
    pos[0] = destDeviceData.m_devicePosition[0];
    pos[1] = destDeviceData.m_devicePosition[1];
    pos[2] = destDeviceData.m_devicePosition[2];
    rpy[0] = destDeviceData.m_gimbalAngles[0];
    rpy[1] = destDeviceData.m_gimbalAngles[1];
    rpy[2] = destDeviceData.m_gimbalAngles[2];
}


/*******************************************************************************
 Checks the state of the gimbal button and gets the position of the device.
*******************************************************************************/
HDCallbackCode HDCALLBACK updateDeviceCallback(void *pUserData)
{
    int nButtons = 0;
    DeviceData *pDeviceData = (DeviceData *) pUserData;

    hdBeginFrame(hdGetCurrentDevice());

    /* Retrieve the current button(s). */
    hdGetIntegerv(HD_CURRENT_BUTTONS, &nButtons);
    
    /* In order to get the specific button 1 state, we use a bitmask to
       test for the HD_DEVICE_BUTTON_1 bit. */
    pDeviceData->m_button1State = 
        (nButtons & HD_DEVICE_BUTTON_1) ? HD_TRUE : HD_FALSE;
 
    /* In order to get the specific button 2 state, we use a bitmask to
       test for the HD_DEVICE_BUTTON_2 bit. */
    pDeviceData->m_button2State = 
        (nButtons & HD_DEVICE_BUTTON_2) ? HD_TRUE : HD_FALSE;
 
    /* Get the current location of the device (HD_GET_CURRENT_POSITION)
       We declare a vector of three doubles since hdGetDoublev returns 
       the information in a vector of size 3. */
    hdGetDoublev(HD_CURRENT_POSITION, pDeviceData->m_devicePosition);

    /* Get the angles of the device gimbal. For Touch
       devices: From Neutral position Right is +, Up is -,
       CW is + . */
    hdGetDoublev(HD_CURRENT_GIMBAL_ANGLES, pDeviceData->m_gimbalAngles);

    /* Also check the error state of HDAPI. */
    pDeviceData->m_error = hdGetError();

    /* Copy the position into our device_data tructure. */
    hdEndFrame(hdGetCurrentDevice());

    return HD_CALLBACK_CONTINUE;    
}


/*******************************************************************************
 Checks the state of the gimbal button and gets the position of the device.
*******************************************************************************/
HDCallbackCode HDCALLBACK copyDeviceDataCallback(void *pUserData)
{
    DeviceSourceDestPointers *pSourceDestPointers =
        (DeviceSourceDestPointers *)pUserData;
    memcpy(pSourceDestPointers->dest, pSourceDestPointers->source,
           sizeof(DeviceData));
    return HD_CALLBACK_DONE;
}


/*******************************************************************************
 Prints out a help string about using this example.
*******************************************************************************/
void printHelp(void)
{
    static const char help[] = {"\
Press and release the stylus button to print out the current device location.\n\
Press and hold the stylus button to exit the application\n"};

    fprintf(stdout, "%s\n", help);
}


