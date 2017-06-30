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

    connect(&receiverHardware, SIGNAL(sigLog(QString,QString)), this, SLOT(slotLog(QString,QString)));
    connect(&receiverHardware, SIGNAL(sigAlert()), this, SLOT(slotAlert()));

    connect(this, SIGNAL(sigGrabNextFrame()), &camera, SLOT(slotPrepareNextFrame()), Qt::QueuedConnection);
    connect(&camera, SIGNAL(sigLog(QString,QString)), this, SLOT(slotLog(QString,QString)), Qt::QueuedConnection);
    connect(&camera, SIGNAL(sigFrameReady(cv::Mat)), this, SLOT(slotFrameReady(cv::Mat)), Qt::QueuedConnection);
    connect(this, SIGNAL(sigStartCamera()), &camera, SLOT(slotStart()), Qt::DirectConnection);

    connect(ui->btnStart, SIGNAL(pressed()), this, SLOT(slotStartCamera()), Qt::QueuedConnection);
    connect(ui->btnStop, SIGNAL(pressed()), &camera, SLOT(slotStop()), Qt::QueuedConnection);
    connect(ui->btnCamConf, SIGNAL(pressed()), &camera, SLOT(slotSelectCamera()), Qt::DirectConnection);

    connect(&receiverTimer, SIGNAL(timeout()), this, SLOT(check()));
    receiverTimer.setSingleShot(true);

    connect(&spliterDefectTimer, SIGNAL(timeout()), this, SLOT(slotSendDefectedCommandToSplitter()));
    spliterDefectTimer.setSingleShot(true);
    connect(&spliterPerfectTimer, SIGNAL(timeout()), this, SLOT(slotSendPerfectCommandToSplitter()));
    spliterPerfectTimer.setSingleShot(true);

    startup();

}

MainWindow::~MainWindow()
{
    closeup();
    delete ui;
}

void MainWindow::slotSendDefectedCommandToSplitter()
{
    if(!this->splitterPort.isOpen())
        return;

    this->splitterPort.write("0");
    this->splitterPort.flush();
}

void MainWindow::slotSendPerfectCommandToSplitter()
{
    if(!this->splitterPort.isOpen())
        return;

    this->splitterPort.write("1");
    this->splitterPort.flush();
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
    QString receiverPortname   = settings->value("receiverHardware/portname", "COM").toString();
    int     receiverBaudrate   = settings->value("receiverHardware/baudrate", 9600).toInt();
    int     receiverDelay      = settings->value("receiverHardware/delay", 0).toInt();
    QString splitterPortname   = settings->value("splitterHardware/portname", "COM").toString();
    int     splitterBaudrate   = settings->value("splitterHardware/baudrate", 9600).toInt();
    int     splitterDelay      = settings->value("splitterHardware/delay", 0).toInt();


    ui->txtPort->setText(receiverPortname);
    ui->txtBaudRate->setValue(receiverBaudrate);
    ui->txtDelay->setValue(receiverDelay);
    ui->txtSplitterPort->setText(splitterPortname);
    ui->txtSplitterBaudRate->setValue(splitterBaudrate);
    ui->txtSplitterDelay->setValue(splitterDelay);

    this->on_btnConnect_clicked();
    this->on_btnSplitterConnect_clicked();
    if(camera.openDevice(cameraName)) {
        slotStartCamera();
    }

}

void MainWindow::closeup()
{
    settings->setValue("camera/name", camera.getDeviceName());
    settings->setValue("receiverHardware/portname", ui->txtPort->text());
    settings->setValue("receiverHardware/baudrate", ui->txtBaudRate->value());
    settings->setValue("receiverHardware/delay", ui->txtDelay->value());
    settings->setValue("splitterHardware/portname", ui->txtSplitterPort->text());
    settings->setValue("splitterHardware/baudrate", ui->txtSplitterBaudRate->value());
    settings->setValue("splitterHardware/delay", ui->txtSplitterDelay->value());
}

void MainWindow::check()
{
    if(!currentFrame.empty()) {

        cv::Mat currentFrameResized;
        cv::resize(currentFrame, currentFrameResized, Size(320, 240));

        bool isDefected = SarShishe::isDefected(currentFrameResized, output);

        if(isDefected) {
            int val = ui->lcdDefect->value(); val++;
            ui->lcdDefect->display(val);
            ui->lblResult->setText("Defected");
            spliterDefectTimer.start(this->ui->txtSplitterDelay->value());
        } else {
            int val = ui->lcdCorrect->value(); val++;
            ui->lcdCorrect->display(val);
            ui->lblResult->setText("Perfect");
            spliterPerfectTimer.start(this->ui->txtSplitterDelay->value());
        }

        showImage(output, ui->lblOutput);
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
    receiverTimer.start(ui->txtDelay->value());
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
    receiverHardware.setSerialPort(ui->txtPort->text(), ui->txtBaudRate->value());
}

void MainWindow::on_btnDisconnect_clicked()
{
    receiverHardware.slotDisconnect();
}

void MainWindow::on_btnResetCounter_clicked()
{
    ui->lcdCorrect->display(0);
    ui->lcdDefect->display(0);
}

void MainWindow::on_btnSplitterConnect_clicked()
{
    if(this->splitterPort.isOpen()) {
        this->splitterPort.close();
    }

    this->splitterPort.setPortName(this->ui->txtSplitterPort->text());

    if(this->splitterPort.open(QIODevice::WriteOnly)) {
        this->splitterPort.setParity(QSerialPort::NoParity);
        this->splitterPort.setBaudRate(this->ui->txtSplitterBaudRate->value());
        this->splitterPort.setStopBits(QSerialPort::OneStop);
        this->splitterPort.setDataBits(QSerialPort::Data8);

        this->slotLog("Splitter hardware", "Successfully connected to the splitter port");
    } else {
        this->slotLog("Splitter hardware", this->splitterPort.errorString());
    }
}
