#ifndef CAMERA_H
#define CAMERA_H
#include <QString>
#include "opencv2/opencv.hpp"
#include <iostream>
#include "tisgrabber.h"
#include <QDebug>
#include <QThread>


class Camera : public QThread
{
    Q_OBJECT
public:
    Camera();
    ~Camera();

    void    initLibrary();
    QString getDeviceName();
    bool    openDevice(QString _name);


protected:
    void    run();

public slots:
    void    slotStart();
    void    slotStop();
    void    slotPrepareNextFrame();
    void    slotSelectCamera();
    void    slotRelease();

signals:
    void    sigLog(QString _title, QString _desc);
    void    sigFrameReady(const cv::Mat  _frame);

private:
    bool        cameraLive;
    bool        cameraOpen;
    QString     Error;
    HGRABBER    hGrabber;
    cv::Size    frameSize;
    cv::Mat     currentFrame;
    QString     deviceName;
};

#endif // CAMERA_H
