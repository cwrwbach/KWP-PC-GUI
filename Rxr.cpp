#define REMOTE

#include "MainWindow.h"
#include "Rxr.h"
#include <vws/websocket.h>
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
vws_cnx* cnx;

char serv_addr[32];
int debug_fft;
//---

int Rxr::setup_kiwi()
{
char uri_string[256];
int debug;
int watch_dog;
// Create connection object
cnx = vws_cnx_new();

int8_t xfer_buf[1040];

//for(int i = 0; i< 1024;i++)
//    xfer_buf[i] = i/4;

//do_network_setup();

// Set connection timeout to 2 seconds (the default is 10). This applies
// both to connect() and to read operations (i.e. poll()).
vws_socket_set_timeout((vws_socket*)cnx, 5);

// Connect. This will automatically use SSL if "wss" scheme is used.
//cstr uri = "ws://localhost:8181/websocket";

time_t utc_now = time( NULL );
printf(" utc %d \n" , utc_now);

//Complete 'GET' header string is:
sprintf(uri_string,"ws://norsom.proxy.kiwisdr.com:8073/%d/W/F",utc_now);
printf("Header string: %s\n",uri_string);

if (vws_connect(cnx, uri_string) == false)
    {
    printf("Failed to connect to the WebSocket server\n");
    vws_cnx_free(cnx);
    return 1;
    }

// Can check connection state this way. 
assert(vws_socket_is_connected((vws_socket*)cnx) == true);

// Enable tracing - dump frames to the console in human-readable format.
vws.tracelevel = VT_PROTOCOL;

//Commands to the KIWISDR to set up a waterfall
// Send a TEXT frame
vws_frame_send_text(cnx, "SET auth t=kiwi p=");
usleep(100000);
vws_frame_send_text(cnx,"SET zoom=8 cf=15000");
usleep(100000);
vws_frame_send_text(cnx,"SET maxdb=0 mindb=-100");
usleep(100000);
vws_frame_send_text(cnx,"SET wf_speed=2");
usleep(100000);
vws_frame_send_text(cnx,"SET wf_comp=0");
usleep(100000);
vws_frame_send_text(cnx,"SET ident_user=Lowa Wather");
printf(" Line %d \n",__LINE__);

//LOOPIN
debug = 0;
watch_dog=0;



while(1)
    {
    // Receive websocket message
    vws_msg* reply = vws_msg_recv(cnx);

    if (reply == NULL)
        {
        printf(" No Message  recd. Line: %d \n",__LINE__);
        // There was no message received and it resulted in timeout
        }
    else
        {
        // Free message
        printf(" Received: %d \n",debug++);
        if(watch_dog++ > 30)
            {
            watch_dog = 0;
            vws_frame_send_text(cnx,"SET keepalive");
            }

        for(int i = 0; i< 1024;i++)
            {
            xfer_buf[i] = reply->data->data[i]; //signed dB
            }
        vws_msg_free(reply);

      //  sendto(sockfd_1, &xfer_buf, FFT_PAK_LEN , 0, (struct sockaddr *) &	cliaddr_1, sizeof(cliaddr_1));
        }
     } //while(1)


//



// temp added 



printf("done kiwi setup %d \n",__LINE__);
usleep(100000);
}



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

