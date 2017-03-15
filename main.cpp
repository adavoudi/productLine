#include "mainwindow.h"
#include <QApplication>
#include <sarshishe.h>
#include <iostream>
#include <QSettings>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    qRegisterMetaType<cv::Mat>("cv::Mat");

    QApplication a(argc, argv);

    QSharedPointer<QSettings> settings(new QSettings("Settings_" + a.applicationName() + ".ini", QSettings::IniFormat));

    MainWindow w(settings);
    w.show();    


    return a.exec();
}
