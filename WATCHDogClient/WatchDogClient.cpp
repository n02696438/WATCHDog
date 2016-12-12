#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

VideoCapture    capture;
Mat             img0, img1, img2;
int             is_data_ready = 1;
int             clientSock;
char*     	server_ip;
int       	server_port;
char*		key_pressed;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* streamClient(void* arg);
void  quit(string msg, int retval);
void*  keyListen(void* arg);

int main(int argc, char** argv)
{
        pthread_t   thread_c,thread_k;
        int         key;

        if (argc < 3) {
                quit("Invalid parameters. Command: WatchDogClient WatchDogServer_ip WatchDogServer_port ", 0);
        }
        if (argc == 4) {
                capture.open(argv[4]);
        } else {
                capture.open(0);
        }

        if (!capture.isOpened()) {
                quit("\n--> cvCapture failed", 1);
        }

        server_ip   = argv[1];
        server_port = atoi(argv[2]);

        capture >> img0;
        img1 = Mat::zeros(img0.rows, img0.cols ,CV_8UC1);
	
        // run the streaming client as a separate thread 
        if (pthread_create(&thread_c, NULL, streamClient, NULL)) {
                quit("\n--> pthread_create failed.", 1);
        }
	if (pthread_create(&thread_k, NULL, keyListen,NULL)){
		quit("\n--> pthread_create failed.", 1);
	}

        cout << "\n--> Press 'q' to quit. \n\n" << endl;

        cout << "\n--> Transferring  (" << img0.cols << "x" << img0.rows << ")  images to the:  " << server_ip << ":" << server_port << endl;

        while(key != 'q') {
		/* get a frame from camera */
                capture >> img0;
                if (img0.empty()) break;

                pthread_mutex_lock(&mutex);

                        flip(img0, img0, 1);
                        cvtColor(img0, img1, CV_BGR2GRAY);

                        is_data_ready = 1;

                pthread_mutex_unlock(&mutex);

                key = waitKey(30);
        }

        //if q is pressed, close client application
        if (pthread_cancel(thread_c)) {
                quit("\n--> pthread_cancel failed.", 1);
        }

     
        quit("\n--> NULL", 0);
return 0;
}

void* keyListen(void* arg)
{
		system("python srd.py");

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * This is the streaming client, run as separate thread
 */
void* streamClient(void* arg)
{
        struct  sockaddr_in serverAddr;
	socklen_t           serverAddrLen = sizeof(serverAddr);

        /* make this thread cancellable using pthread_cancel() */
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

        if ((clientSock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            quit("\n--> socket() failed.", 1);
        }

        serverAddr.sin_family = PF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(server_ip);
        serverAddr.sin_port = htons(server_port);

        if (connect(clientSock, (sockaddr*)&serverAddr, serverAddrLen) < 0) {
                quit("\n--> connect() failed.", 1);
        }

        int  imgSize = img1.total()*img1.elemSize();
        int  bytes=0;
        img2 = (img1.reshape(0,1)); // to make it continuous

        /* start sending images */
        while(1)
        {
                /* send the grayscaled frame, thread safe */
                if (is_data_ready) {
                        pthread_mutex_lock(&mutex);
                                if ((bytes = send(clientSock, img2.data, imgSize, 0)) < 0){
					cerr << "\n--> bytes = " << bytes << endl;
                                	quit("\n--> send() failed", 1);
				}
				is_data_ready = 0;
                        pthread_mutex_unlock(&mutex);
			memset(&serverAddr, 0x0, serverAddrLen);
                }
                /* if something went wrong, restart the connection */
                if (bytes != imgSize) {
                        cerr << "\n-->  Connection closed (bytes != imgSize)" << endl;
                        close(clientSock);

                        if (connect(clientSock, (sockaddr*) &serverAddr, serverAddrLen) == -1) {
                                quit("\n--> connect() failed", 1);
                        }
                }
		
                /* have we terminated yet? */
                pthread_testcancel();

                /* no, take a rest for a while */
                usleep(1000);   //1000 Micro Sec
        }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void quit(string msg, int retval)
{
        if (retval == 0) {
                cout << (msg == "NULL" ? "" : msg) << "\n" << endl;
        } else {
                cerr << (msg == "NULL" ? "" : msg) << "\n" << endl;
        }
        if (clientSock){
                close(clientSock);
        }
        if (capture.isOpened()){
                capture.release();
        }
        if (!(img0.empty())){
                (~img0);
        }
        if (!(img1.empty())){
                (~img1);
        }
        if (!(img2.empty())){
                (~img2);
        }
        pthread_mutex_destroy(&mutex);
        exit(retval);
}




