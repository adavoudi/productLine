#include "camera.h"

Camera::Camera() : cameraOpen(false), cameraLive(false)
{
    start();
    moveToThread(this);
}

Camera::~Camera()
{
    slotRelease();
}

void Camera::initLibrary()
{
    if(!IC_InitLibrary(0)){
        emit sigLog("camera", "Initialization failed");
    }
}

QString Camera::getDeviceName()
{
    return deviceName;
}

bool Camera::openDevice(QString _name)
{
    if(cameraLive)
        slotStop();

    char *__name = strdup(_name.toStdString().c_str());
    QString test(__name);
    qDebug() << test;
    HGRABBER hGrabber = IC_CreateGrabber();

    if(hGrabber != NULL &&
       IC_OpenVideoCaptureDevice(hGrabber, __name) == IC_SUCCESS )
    {
        this->frameSize = cv::Size(IC_GetVideoFormatWidth(hGrabber),IC_GetVideoFormatHeight(hGrabber));
        this->hGrabber  = hGrabber;
        cameraOpen  = true;

        deviceName = _name;

        emit sigLog("camera", "Camera \"" + _name + "\" successfully opened");

        return true;

    } else {
        cameraOpen  = false;
        deviceName = "";

        emit sigLog("camera", "Could not open camera \"" + _name + "\"");
        return false;
    }
}

void Camera::slotSelectCamera()
{
    if(cameraLive)
        slotStop();

    HGRABBER hGrabber = IC_ShowDeviceSelectionDialog(NULL); // Show the built in device select dialog

    if(hGrabber != NULL && IC_GetVideoFormatWidth(hGrabber) > 0 )
    {
        this->frameSize = cv::Size(IC_GetVideoFormatWidth(hGrabber),IC_GetVideoFormatHeight(hGrabber));
        this->hGrabber  = hGrabber;

        deviceName = QString(IC_GetDeviceName(hGrabber));

        cameraOpen  = true;

    } else {
        deviceName = "";
        cameraOpen = false;

        emit sigLog("camera", "No camera selected");
    }
}

void Camera::slotRelease()
{
    if(cameraOpen) {
        if(cameraLive)
            slotStop();

        IC_ReleaseGrabber(&hGrabber);
        cameraOpen = false;
    } else {
        emit sigLog("camera", "No camera selected");
    }
}

void Camera::run()
{
    exec();
}

void Camera::slotStart()
{
    if(cameraOpen) {
        IC_StartLive(hGrabber, 0);
        cameraLive = true;
    } else {
        emit sigLog("camera", "No camera selected");
    }
}

void Camera::slotStop()
{
    if(cameraLive) {
        IC_StopLive(hGrabber);
        cameraLive     = false;
    } else {
        emit sigLog("camera", "No camera is live to stop");
    }
}

void Camera::slotPrepareNextFrame()
{
    if(cameraLive && IC_SnapImage(hGrabber, 2000) == IC_SUCCESS) {
        currentFrame.create(frameSize, CV_8UC3);
        currentFrame.data = IC_GetImagePtr(hGrabber );
        cv::flip(currentFrame, currentFrame, 0);

        emit sigFrameReady(currentFrame);

    } else {
        emit sigLog("camera", "No camera is live to capture frame. check connectivity");
    }
}

