#include <string>
#include <vector>
#include <stdio.h>
#include <QtGui>
#include <QtWidgets/QAction>
#include <math.h>
#include <complex.h>
#include "MainWindow.h"
#include <unistd.h>
#include "ui_QtBase-001.h"
#include "Rxr.h"
#include <QUdpSocket>

#define VERSION "QT5 Scope"
#define FFT_POINTS 1024
#define RX_BUF_SIZE 1024

extern bool stream_flag;
extern int fft_video_buf[];
int status[32]; //FIXME
Rxr radio_rx;

MainWindow::MainWindow(const QString cfgfile, QWidget *parent) :
    QMainWindow(parent),    ui(new Ui::MainWindow)
{
ui->setupUi(this);
printf(" Starting ZXP \n");    
printf("Version: %s\n", GIT_VERSION);
printf(" Line %d \n",__LINE__);

setWindowTitle(QString("ZXP GUI %1 ").arg(GIT_VERSION));
//setWindowTitle(QString(VERSION));

QObject::connect(ui->setupRsp, SIGNAL(clicked()), this, SLOT(hardware_setup()));
 
connect(ui->freqCtrl, SIGNAL(newFrequency(qint64)), this, SLOT(setNewFrequency(qint64)));
connect(ui->alpha_plotter, SIGNAL(newFrequency(qint64)), this, SLOT(setNewFrequency(qint64)));

connect(ui->wf0, SIGNAL(clicked()), this, SLOT(set_wf0()));
connect(ui->wf1, SIGNAL(clicked()), this, SLOT(set_wf1()));
connect(ui->wf2, SIGNAL(clicked()), this, SLOT(set_wf2()));
connect(ui->wf3, SIGNAL(clicked()), this, SLOT(set_wf3()));
connect(ui->wf4, SIGNAL(clicked()), this, SLOT(set_wf4()));

connect(ui->zoom, SIGNAL(valueChanged(int)), this, SLOT(set_zoom(int)));

fft_timer = new QTimer(this);
connect(fft_timer, SIGNAL(timeout()), this, SLOT(show_enable()));
fft_timer->start(50); //milli secs
}

MainWindow::~MainWindow()
{
fft_timer->stop();
delete fft_timer;
delete ui;
}

void MainWindow::hardware_setup()
{
printf("hardware setup started... \n");
radio_rx.setup_socket();
usleep(200000);
}	

void MainWindow::setNewFrequency(qint64 newfreq)
{
int send_cf = newfreq;	
ui->freqCtrl->setFrequency(newfreq);
ui->alpha_plotter->setCenterFreq(send_cf);
radio_rx.update_radio_cf(send_cf);
}

void MainWindow::set_zoom(int zoom_val)
{
radio_rx.update_zoom(zoom_val);
}

void MainWindow::set_wf0()
    { radio_rx.update_wf(0);}
void MainWindow::set_wf1()
    { radio_rx.update_wf(1);}
void MainWindow::set_wf2()
    { radio_rx.update_wf(2);}
void MainWindow::set_wf3()
    { radio_rx.update_wf(3);}
void MainWindow::set_wf4()
    { radio_rx.update_wf(4);}

void MainWindow::show_enable() // displays stream data
{
if(stream_flag ==true)
    {
    stream_flag=false;
    ui->alpha_plotter->draw_trace(fft_video_buf,0,1024); //(left,Num points)
    }
}
