#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <sarshishe.h>
#include <QLabel>
#include <camera.h>
#include <QSettings>
#include <QLocale>
#include <QDateTime>
#include <hardwarereader.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QSharedPointer<QSettings> _settings, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slotFrameReady(const cv::Mat _frame);
    void slotLog(QString _title, QString _desc);
    void slotStartCamera();
    void slotAlert();
    void check();


signals:
    void sigGrabNextFrame();
    void sigStartCamera();

private slots:
    void on_btnSave_clicked();
    void on_btnCheck_clicked();
    void on_btnConnect_clicked();
    void on_btnDisconnect_clicked();

    void on_btnResetCounter_clicked();

private:
    void showImage(Mat &src, QLabel *lbl);
    void startup();
    void closeup();

private:
    Ui::MainWindow          *ui;

    cv::Mat                 currentFrame;
    Camera                  camera;
    cv::Mat                 output;

    hardwareReader          hardware;
    QTimer                  timer;


    QSharedPointer<QSettings>   settings;
};

#endif // MAINWINDOW_H
