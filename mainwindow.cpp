#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>

MainWindow::MainWindow(QSharedPointer<QSettings> _settings, QWidget *parent) :
    QMainWindow(parent), settings(_settings),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    camera.initLibrary();

    connect(&hardware, SIGNAL(sigLog(QString,QString)), this, SLOT(slotLog(QString,QString)));
    connect(&hardware, SIGNAL(sigAlert()), this, SLOT(slotAlert()));

    connect(this, SIGNAL(sigGrabNextFrame()), &camera, SLOT(slotPrepareNextFrame()), Qt::QueuedConnection);
    connect(&camera, SIGNAL(sigLog(QString,QString)), this, SLOT(slotLog(QString,QString)), Qt::QueuedConnection);
    connect(&camera, SIGNAL(sigFrameReady(cv::Mat)), this, SLOT(slotFrameReady(cv::Mat)), Qt::QueuedConnection);
    connect(this, SIGNAL(sigStartCamera()), &camera, SLOT(slotStart()), Qt::DirectConnection);

    connect(ui->btnStart, SIGNAL(pressed()), this, SLOT(slotStartCamera()), Qt::QueuedConnection);
    connect(ui->btnStop, SIGNAL(pressed()), &camera, SLOT(slotStop()), Qt::QueuedConnection);
    connect(ui->btnCamConf, SIGNAL(pressed()), &camera, SLOT(slotSelectCamera()), Qt::DirectConnection);

    connect(&timer, SIGNAL(timeout()), this, SLOT(check()));
    timer.setSingleShot(true);

    startup();

}

MainWindow::~MainWindow()
{
    closeup();
    delete ui;
}

void MainWindow::showImage(Mat &src, QLabel *lbl)
{
    QImage img;
    if(src.channels() == 1)
        img= QImage((const uchar*)(src.data), src.cols, src.rows, src.step, QImage::Format_Indexed8);
    else{
        cvtColor(src, src, CV_BGR2RGB);
        img= QImage((const uchar*)(src.data), src.cols, src.rows, src.step, QImage::Format_RGB888);
    }
    lbl->setPixmap(QPixmap::fromImage(img).scaled(lbl->size(), Qt::KeepAspectRatio));
}

void MainWindow::startup()
{
    QString cameraName = settings->value("camera/name", "DFK").toString();
    QString portname   = settings->value("hardware/portname", "COM").toString();
    int     baudrate   = settings->value("hardware/baudrate", 9600).toInt();
    int     delay      = settings->value("hardware/delay", 0).toInt();

    ui->txtPort->setText(portname);
    ui->txtBaudRate->setValue(baudrate);
    ui->txtDelay->setValue(delay);


    hardware.setSerialPort(portname, baudrate);
    if(camera.openDevice(cameraName)) {
        slotStartCamera();
    }

}

void MainWindow::closeup()
{

    settings->setValue("camera/name", camera.getDeviceName());
    settings->setValue("hardware/portname", ui->txtPort->text());
    settings->setValue("hardware/baudrate", ui->txtBaudRate->value());
    settings->setValue("hardware/delay", ui->txtDelay->value());
}

void MainWindow::check()
{
    if(!currentFrame.empty()) {

        cv::Mat currentFrameResized;
        cv::resize(currentFrame, currentFrameResized, Size(320, 240));

        bool isDefected = SarShishe::isDefected(currentFrameResized, output);

        showImage(output, ui->lblOutput);

        if(isDefected) {
            int val = ui->lcdDefect->value(); val++;
            ui->lcdDefect->display(val);
            ui->lblResult->setText("Defected");
        } else {
            int val = ui->lcdCorrect->value(); val++;
            ui->lcdCorrect->display(val);
            ui->lblResult->setText("Perfect");
        }
    }
}

void MainWindow::slotFrameReady(const Mat _frame)
{
    currentFrame = _frame.clone();
    showImage(currentFrame, ui->lblSrc);

    emit sigGrabNextFrame();
}

void MainWindow::slotLog(QString _title, QString _desc)
{
    static QLocale  locale(QLocale::English, QLocale::UnitedStates);

    QString log = QString("[%1] [%2] %3").
            arg(locale.toString(QDateTime::currentDateTime(), "dd/MM/yyyy hh:mm:ss")).
            arg(_title).
            arg(_desc);

    ui->lstLog->addItem(log);

    QFile logFile("app.log");
    if(logFile.open(QIODevice::Append)) {

        QTextStream logStream(&logFile);
        logStream << log << "\r\n";

        logFile.close();
    }

}

void MainWindow::slotStartCamera()
{
    emit sigStartCamera();
    emit sigGrabNextFrame();
}

void MainWindow::slotAlert()
{
    timer.start(ui->txtDelay->value());
}


void MainWindow::on_btnSave_clicked()
{

    if(output.empty())
        return;

    QString path = QFileDialog::getSaveFileName(this, "Save Image", ".", "Image files (*.png *.jpg)");

    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, "Oops", "The selected file is not writable!!!");
        return;
    }
    file.close();

    imwrite(path.toStdString(), output);
}

void MainWindow::on_btnCheck_clicked()
{
    check();
}


void MainWindow::on_btnConnect_clicked()
{
    hardware.setSerialPort(ui->txtPort->text(), ui->txtBaudRate->value());
}

void MainWindow::on_btnDisconnect_clicked()
{
    hardware.slotDisconnect();
}

void MainWindow::on_btnResetCounter_clicked()
{
    ui->lcdCorrect->display(0);
    ui->lcdDefect->display(0);
}
