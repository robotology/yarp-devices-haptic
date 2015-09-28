#pragma once
#include <HD/hd.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>


/* Holds data retrieved from HDAPI. */
typedef struct 
{
    HDboolean m_button1State;      /* Has the device button has been pressed. */
    HDboolean m_button2State;      /* Has the device button has been pressed. */
    hduVector3Dd m_devicePosition; /* Current device coordinates in mm. */
    hduVector3Dd m_gimbalAngles;   /* Gimbal Angles in rad.*/
    HDErrorInfo m_error;

} DeviceData;

typedef struct
{
    DeviceData *source;
    DeviceData *dest;

} DeviceSourceDestPointers;

/**
 * This class rapresents a GeoMagic Device
 */
class GeoMagic
{
public:
    /**
     * GeoMagic Constructor:
     * creates a 'HD_DEFAULT_DEVICE' GeoMagic Device and 
     * schedules the callback to update device state and positions.
     */
    GeoMagic(void);
    /**
     * GeoMagic destructor.
     */
    ~GeoMagic(void);

    /**
     * This member checks GeoMagic validity and activation.
     * @return true if the device is active, false otherwise
     */
    bool isActive();
    /**
     * This member returns device attributes:
     * @param pos: a 3 elements array for X,Y,Z position in millimeters in device coordinates.
     * @param rpy: a 3 elements array for roll, pitch, yaw in radians.
     * @param b1: button 1 state.
     * @param b2: button 2 state.
     */
    void getDeviceData(double *pos, double *rpy, int *b1, int *b2);

private:
    bool active;
    HHD hHD;
    HDSchedulerHandle hUpdateHandle;
    DeviceData deviceData;
};



