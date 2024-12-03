#include "MainWindow.h"
#include "Rxr.h"
#include <QDebug>
#include <unistd.h>
#include <math.h>
//#include <alsa/asoundlib.h>

#define SERV_ADDR "192.168.2.36" //Local loopback for development

#define AUDIO_RATE 8000
//#define AUDIO_RATE 7812 //11960 //7812  set to silly low rate for debugging

bool stream_flag;
int fft_video_buf[1024];
QVector<quint32> buffer(256);

char serv_addr[32];
int debug_fft;
//---

void Rxr::setup_socket()
{
socket = new QUdpSocket(this);

strcpy(serv_addr,SERV_ADDR);

setup_sound();
usleep(100000);//FIXME - probably not needed

bool result =  socket->bind(QHostAddress::AnyIPv4, 11366);
qDebug() << result;

usleep(100000); //FIXME - probably not needed
connect(socket, &QUdpSocket::readyRead,
            this, &Rxr::processPendingDatagrams);
sendgram();
usleep(100000);//FIXME - probably not needed
}


void Rxr::sendgram()
{
QString word="Sign on message";
QByteArray buffer;
buffer.resize(socket->pendingDatagramSize());
buffer=word.toUtf8();
socket->writeDatagram(buffer.data(), QHostAddress(serv_addr), 11366 );
}


void Rxr::processPendingDatagrams()
 {
int size;
//unsigned char id_type;
QByteArray datagram;   
QHostAddress sender;
u_int16_t port;

while (socket->hasPendingDatagrams())
    {
    size = socket->pendingDatagramSize(); size=size;
    datagram.resize(socket->pendingDatagramSize());
    socket->readDatagram(datagram.data(),datagram.size(),&sender,&port);

 //printf("Size of pkt rxd %d Type: 0x%x \n",size,id_type);     
    //if(id_type == 0x42) //FFT
    if(1)
        { 
         for(int i=0; i<1024;i++)
            fft_video_buf[i] = ((int) datagram[i+HEADER_LEN]);
        stream_flag = true;
        }
    }   
}

void Rxr::setup_sound()
{
//int err;
//int audio_sr = AUDIO_RATE;
//printf(" Setup sound device\n");
//strcpy(alsa_device,"default");
//err = snd_pcm_open(&audio_device, alsa_device, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
//err=err; //0
//printf("err %d %d\n",err,__LINE__);
//err = snd_pcm_set_params(audio_device,SND_PCM_FORMAT_A_LAW, SND_PCM_ACCESS_RW_INTERLEAVED,1,audio_sr,1,400000);
//err=err;
//latency in
//printf("err %d %d\n",err,__LINE__);
}

void Rxr::update_radio_cf(int cf )
{
float ppm_factor, freq;

ppm_factor = 0.0;
freq = cf;
freq = (int)floor(freq*(1.0 + ppm_factor *1.0e-6) + 0.5);
buffer[FREQ]=(int) freq;
socket->writeDatagram((char*)buffer.data(),buffer.size()*sizeof(int),QHostAddress(serv_addr),11366);
}	


void Rxr::start_server_stream(){};

/*
void Rxr::update_radio_sr(int){};
void Rxr::update_radio_ar(int){};


void Rxr::update_radio_rfg(int val)
{
buffer[RFG]= val;
socket->writeDatagram((char*)buffer.data(),buffer.size()*sizeof(int),QHostAddress(serv_addr),11366);
//printf("radio rfg: %d\n",val);
}

*/

void Rxr::update_zoom(int val)
{
char msg_buf[32];
int len;

sprintf(msg_buf,"SET zoom=%d cf=15000",val);
len = strlen(msg_buf);

printf (" LEN = %d \n",len);

socket->writeDatagram((char*)msg_buf,len,QHostAddress(serv_addr),11366);

//socket->writeDatagram((char*)buffer.data(),buffer.size()*sizeof(int),QHostAddress(serv_addr),11366);
printf("Zoom value: %d ::: %s\n",val,msg_buf);
}

void Rxr::update_wf(int val)
{
char msg_buf[32];

sprintf(msg_buf,"SET wf_speed=%d",val);

//socket->writeDatagram((char*)buffer.data(),buffer.size()*sizeof(int),QHostAddress(serv_addr),11366);

socket->writeDatagram((char*)msg_buf,14,QHostAddress(serv_addr),11366);

printf(" Update WF %d ::: %s \n",val,msg_buf); 
}    

