#pragma once

#include <QMainWindow>
#include <QUdpSocket>
//#include <alsa/asoundlib.h>

#define FREQ 4

#define PAK_LEN 1280
#define HEADER_LEN 32
#define DATA_LEN 1024

class Rxr : public QObject
{
    Q_OBJECT
    QUdpSocket *socket = nullptr;
    //snd_pcm_t *audio_device;
    void setup_sound();
    char alsa_device[64];

public:
    int setup_kiwi();
    void setup_socket();
    void sendgram();

    void update_radio_cf(int);
    void update_wf(int);
    void update_zoom(int);

public slots:    
    void processPendingDatagrams();
    void start_server_stream();
    
};


