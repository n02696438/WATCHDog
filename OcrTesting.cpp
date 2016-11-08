#include "OcrHeader.h"

int main(int argc, char** argv)
{
	/*pthread_t thread_s;
    int width, height, key;
	width = 640;  
	height = 480; 
 	
	if (argc != 2)
		quit("Usage: netcv_server <listen_port> ", 0);
	
	listenPort = atoi(argv[1]);
 	
    img = Mat::zeros( height,width, CV_8UC1);*/
        
    /* run the streaming server as a separate thread */
    /*if (pthread_create(&thread_s, NULL, streamServer, NULL))
    	quit("pthread_create failed.", 1);
    
    cout << "\n-->Press 'q' to quit." << endl;
    namedWindow("stream_server", CV_WINDOW_AUTOSIZE);*/
		
	/*api = new tesseract::TessBaseAPI();
   	// Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init(NULL, "eng")) {
    	fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }


    while(key != 'q')
	{
		pthread_mutex_lock(&mutex);
        if (is_data_ready) 
		{
			cv::flip(img,img,1);
			//img = DetectAndDisplay(img);
            imshow("stream_server", img);
			is_data_ready = 0;					
        }
		if(key == 'c')
		{
			try{
				cropForOCR(img);}
			catch(exception e){
				std::cout << "Error during License OCR" <<std::endl;}
		}
        pthread_mutex_unlock(&mutex);
        key = waitKey(10);
	}

    if (pthread_cancel(thread_s))
    	quit("pthread_cancel failed.", 1);

    destroyWindow("stream_server");
    quit("NULL", 0);*/
	api = new tesseract::TessBaseAPI();
	api->Init(NULL, "eng");
	//-- 1. Load the cascades
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
    if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
	Mat imgForOCR = imread("image.png", 1);
	if(! imgForOCR.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
	imgForOCR = detectAndDisplay(imgForOCR);
	cropForOCR(imgForOCR);
	
	waitKey(0);
	
	//return 0;
}

void cropForOCR(Mat LicenseImg)
{
	//Mat LicenseImgGray,LicenseImgFinal;
	cvtColor( LicenseImg, src_gray, CV_BGR2GRAY);

    /// Create a window for sliders
	namedWindow( Threshold_Window, CV_WINDOW_AUTOSIZE );
	setMouseCallback(Threshold_Window,onMouse,NULL );
	//Create Trackbar to set ID Threshold image level
	createTrackbar( trackbar_IDValue,Threshold_Window, &threshold_ID_value,max_value, Threshold );
	//Create Trackbar to set Name Threshold image level
	createTrackbar( trackbar_NameValue,Threshold_Window, &threshold_Name_value,max_value, Threshold );
	//Create Trackbar to set threshold done
	createTrackbar( trackbar_Done, Threshold_Window, &threshold_done,1, Threshold );
	createTrackbar( trackbar_value,Threshold_Window, &threshold_value,max_value, Threshold );

	
	
	Threshold( 0, 0 );
}

void Threshold(int, void*)
{
	//Perform threshold, show window with sliders to adjust
	threshold(src_gray,dst,threshold_value, max_BINARY_value,threshold_type );
  	imshow(Threshold_Window,dst);
	//Assign slider values to global variables
	IDThresh = threshold_ID_value;
	NameThresh = threshold_Name_value;
}


void onMouse( int event, int x, int y, int f, void* ){


    switch(event){

        case  CV_EVENT_LBUTTONDOWN  :
                                        clicked=true;

                                        P1.x=x;
                                        P1.y=y;
                                        P2.x=x;
                                        P2.y=y;
                                        break;

        case  CV_EVENT_LBUTTONUP    :
                                        P2.x=x;
                                        P2.y=y;
                                        clicked=false;
										nameSelected = 1;
                                        break;

        case  CV_EVENT_MOUSEMOVE    :
                                        if(clicked){
                                        P2.x=x;
                                        P2.y=y;
                                        }
                                        break;

        default                     :   break;


    }


    if(clicked){
     if(P1.x>P2.x){ cropRect.x=P2.x;
                       cropRect.width=P1.x-P2.x; }
        else {         cropRect.x=P1.x;
                       cropRect.width=P2.x-P1.x; }

        if(P1.y>P2.y){ cropRect.y=P2.y;
                       cropRect.height=P1.y-P2.y; }
        else {         cropRect.y=P1.y;
                       cropRect.height=P2.y-P1.y; }
    }

	showImage();
}


void showImage(){
    Mat temp =src.clone();
    if(cropRect.width>0&&cropRect.height>0){
        ROI=src_gray(cropRect);//changed dst to src_gray
        imshow("cropped",ROI);
    }

    rectangle(temp, cropRect, Scalar(0,255,0), 1, 8, 0 );
	if(nameSelected == 1)
	{
		int cropHeight = ROI.size().height;
		int cropWidth = ROI.size().width;
		threshold(ROI(Rect(cropWidth*.38,cropHeight*.45,cropWidth*.24,cropHeight*.16)),NameFrame,NameThresh, max_BINARY_value,threshold_type );
		threshold(ROI(Rect(cropWidth*.44,cropHeight*.35,cropWidth*.28,cropHeight*.12)),IDFrame,IDThresh, max_BINARY_value,threshold_type );
		imwrite("CroppedL.png",ROI);
		//imwrite("CroppedID.png",ROI(Rect(cropWidth*.44,cropHeight*.35,cropWidth*.28,cropHeight*.12)));
		//imwrite("CroppedName.png",ROI(Rect(cropWidth*.38,cropHeight*.45,cropWidth*.24,cropHeight*.16)));
		imwrite("CroppedID.png",IDFrame);
		imwrite("CroppedName.png",NameFrame);
		cvDestroyWindow("cropped");
		cvDestroyWindow(Threshold_Window);
		nameSelected = 0;
		cropDone = 1;
		//cropRect=[0,0,0,0];
		Pix *image = pixRead(IDImageForOCR);
	   	api->SetImage(image);
	   	// Get OCR result
	   	STRING IDText = api->GetUTF8Text();
		image = pixRead(NameImageForOCR);
		api->SetImage(image);
		// Get OCR result
	   	STRING NameText = api->GetUTF8Text();
		std::cout << "ID Number: " << IDText.string() << std::endl;
		std::cout << "Name: " << NameText.string() << std::endl;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * This is the streaming server, run as separate thread
 */
void* streamServer(void* arg)
{
        struct  sockaddr_in   serverAddr,  clientAddr;
        socklen_t             clientAddrLen = sizeof(clientAddr);

        /* make this thread cancellable using pthread_cancel() */
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

        if ((listenSock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            quit("socket() failed.", 1);
        }
            
        serverAddr.sin_family = PF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(listenPort);

        if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
                quit("bind() failed", 1);
        }
                
        if (listen(listenSock, 5) == -1) {
                quit("listen() failed.", 1);
        }
        
        int  imgSize = img.total()*img.elemSize();
        char sockData[imgSize];
        int  bytes=0;
                
        /* start receiving images */
        while(1)
        {
	        cout << "-->Waiting for TCP connection on port " << listenPort << " ...\n\n";
	  	
		/* accept a request from a client */
	        if ((connectSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
	                quit("accept() failed", 1);
	        }else{
		    	cout << "-->Receiving image from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "..." << endl;
		}
		
		memset(sockData, 0x0, sizeof(sockData));
		
		while(1){

                	for (int i = 0; i < imgSize; i += bytes) {
                        	if ((bytes = recv(connectSock, sockData +i, imgSize  - i, 0)) == -1) {
 	                              	quit("recv failed", 1);
				}
                	}
                	/* convert the received data to OpenCV's Mat format, thread safe */
                	pthread_mutex_lock(&mutex);
                        	for (int i = 0;  i < img.rows; i++) {
                        	        for (int j = 0; j < img.cols; j++) {
                        	                (img.row(i)).col(j) = (uchar)sockData[((img.cols)*i)+j];
                        	        }
                        	}
                        	is_data_ready = 1;
				memset(sockData, 0x0, sizeof(sockData));
                	pthread_mutex_unlock(&mutex);
		}
	}

        /* have we terminated yet? */
        pthread_testcancel();
	
  	/* no, take a rest for a while */
        usleep(1000);
	
}
/////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * This function provides a way to exit nicely from the system
 */
void quit(string msg, int retval)
{
        if (retval == 0) {
                cout << (msg == "NULL" ? "" : msg) << "\n" <<endl;
        } else {
                cerr << (msg == "NULL" ? "" : msg) << "\n" <<endl;
        }
         
        if (listenSock){
                close(listenSock);
        }

        if (connectSock){
                close(connectSock);
        }
                                
        if (!img.empty()){
                (img.release());
        }
                
        pthread_mutex_destroy(&mutex);
        exit(retval);
}


Mat detectAndDisplay( Mat frame )
{
	std::vector<Rect> faces;
	Mat frame_gray;
	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );
	//-- Detect faces
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
	//draw circles surrounding detected faces
	for( size_t i = 0; i < faces.size(); i++ )
	{
		Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
		Mat faceROI = frame_gray( faces[i] );
	}
	//return Mat frame with faces detected
	return frame;
}

