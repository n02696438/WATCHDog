#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <tesseract/baseapi.h>
#include <tesseract/strngs.h>
#include <leptonica/allheaders.h>
using namespace std;
using namespace cv;

//Variables for streaming server                     
Mat     img;
int     is_data_ready = 0;
int     listenSock, connectSock;
int 	listenPort;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

///Threshold Slider VARIABLES
int threshold_value = 130;
int threshold_type = 0;
int threshold_ID_value = 130;
int threshold_Name_value = 110;
int threshold_done = 0;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;
//ID Image Threshold Value
int IDThresh = 145;
//Name image Threshold Value
int NameThresh = 145;
//thresholding sliders for setup/testing
const char* Threshold_Window = "Threshold";
const char* trackbar_IDValue = "ID Thresh Value";
const char* trackbar_NameValue = "Name Thresh Value";
const char* trackbar_Done = "0 = not done 1 = done";
const char* trackbar_value = "Value";
const char* IDImageForOCR = "CroppedID.png";
const char* NameImageForOCR = "CroppedName.png";

//Image processing Mat variables
Mat src, src_gray, dst;

//function prototypes for thresholding
void Threshold( int, void* ); //threshold function header
//variables for interactive cropping
static Mat ROI;
static Rect cropRect(0,0,0,0);
static Point P1(0,0);
static Point P2(0,0);
static int nameSelected = 0;
static int cropDone = 0;

//Function and Variables for Facial Detection
Mat detectAndDisplay( Mat frame );
String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
RNG rng(12345);
Mat NameFrame, IDFrame;

const char* winName="Crop Image";
bool clicked=false;
void checkBoundary(void);
void showImage(void);
void onMouse(int,int,int,int,void*);
void cropForOCR(Mat);

void* streamServer(void* arg);
void  quit(string msg, int retval);
static tesseract::TessBaseAPI *api;
