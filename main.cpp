#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "HorizonLineDetector.h"
#include <stdlib.h>

using namespace std;
using namespace cv;

CascadeClassifier cc[2];
HorizonLineDetector hld;
double hldfactor = 0.1;
double haarfactor = 0.4;
int VideoFlag = 0;


void init(){
	cc[0].load("data/front.xml");
    cc[1].load("data/profile.xml");
	hld.set_canny_param(40);
    hld.set_max_search_steps(10);
    hld.init_detector("data/saved_model.svm");
}

bool in(CvRect a, CvRect b){
	int x1 = max(a.x, b.x);
	int y1 = max(a.y, b.y);
	int x2 = min(a.x+a.width, b.x+b.width);
	int y2 = min(a.y+a.height, b.y+b.height);
	if( x1 > x2 || y1 > y2 ) 
		return false;
	else
		return true;
}

bool sort_by_w(const FaceResult a, const FaceResult b){
	if( a.dir < b.dir ) return true;
	return a.weight > b.weight;
}

void faceDetect(Mat Oframe, vector<FaceResult>& ret){
	vector<Rect> faces;
	vector<int> weights;
	Mat frame;
	int n;
	resize(Oframe, frame, Size(), haarfactor, haarfactor, CV_INTER_LINEAR);
	cvtColor(frame, frame, CV_RGB2GRAY);
	equalizeHist(frame, frame);

	cc[0].detectMultiScale(frame, faces, weights, 1.05, 6, 0);
	n = faces.size();
	for(int i = 0; i < n; ++i)
		ret.push_back(FaceResult(
					  CvRect(faces[i].tl().x/haarfactor,faces[i].tl().y/haarfactor,
   					  faces[i].width/haarfactor,faces[i].height/haarfactor), weights[i], 1));


	cc[1].detectMultiScale(frame, faces, weights, 1.05, 20, 0);
	n = faces.size();
	for(int i = 0; i < n; ++i)
		ret.push_back(FaceResult(CvRect(faces[i].tl().x/haarfactor,faces[i].tl().y/haarfactor,
   					  faces[i].width/haarfactor,faces[i].height/haarfactor), weights[i], 2));

	

	Mat ff;
	flip(frame, ff, 1);
	cc[1].detectMultiScale(ff, faces, weights, 1.05, 20, 0);
	n = faces.size();
	for(int i = 0; i < n; ++i){
		int X = ff.cols - faces[i].tl().x - faces[i].width - 1;
		int Y = faces[i].tl().y;
		ret.push_back(FaceResult(CvRect(X/haarfactor, Y/haarfactor, faces[i].width/haarfactor, faces[i].height/haarfactor), weights[i],3));
	}

	vector<FaceResult>::iterator it1;
	vector<FaceResult>::iterator it2;
	for(it1 = ret.begin(); it1 != ret.end(); ){
		bool succ = 0;
		for(it2 = ret.begin(); it2 != ret.end(); it2++ ){
			if( it1 == it2 ) continue;
			if( in((*it1).face, (*it2).face) ) {
				succ = 1;
				break;			
			}
		}
		if(succ == 1)
			it1 = ret.erase(it1);
		else
			++it1;
	}
		
	sort(ret.begin(), ret.end(), sort_by_w);
}

void HLD(Mat Oframe){
	Mat mask; //actually it's empty;
	Mat frame;
	resize(Oframe, frame, Size(), hldfactor, hldfactor, CV_INTER_LINEAR);
	hld.detect_image(frame,mask);
    hld.draw_horizon();
	//imshow("hld", hld.current_draw);
}

void processFace(Mat img, vector<FaceResult>& faces){
	int n = faces.size();
	if( n == 0 ) return;
	for( int i = 0; i < n; ++i)
		rectangle(img, faces[i].face, Scalar(255,0,0), 2, 1);
}

void drawFps(Mat img, double t){
	t = ((double)getTickCount() - t) / getTickFrequency();
	double fps = 1.0/t;
	char content[100];
 	sprintf(content, "fps : %5f", fps);
 	putText(img, content, Point(0,15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,255));
}

void processWface(Mat img, vector<FaceResult>& faces){
	if(faces.size() == 0) return;
	Scalar s;
	printf("%d\n", faces[0].dir);
	if(faces[0].dir == 1)
		s = Scalar(255,0,0);
	if(faces[0].dir == 2)
		s = Scalar(0,255,0);
	if(faces[0].dir == 3)
		s = Scalar(0,0,255);

	rectangle(img, faces[0].face, s, 2, 1);
	imshow("Wface", img);
	
}

int main(int argc, char* argv[]){
	//Creat window and open camera
    string windowName = "Camera";
    namedWindow(windowName, WINDOW_AUTOSIZE);
	VideoFlag = (argc>1);
	VideoCapture cap;
	VideoWriter outputVideo;
	if(VideoFlag){ 
    	cap.open(argv[1]);
		outputVideo.open("data/save.avi", CV_FOURCC('P','I','M','1'), 20, Size(1920,1080), true);
	}
	else{
		cap.open(0);
		outputVideo.open("data/save.avi", CV_FOURCC('P','I','M','1'), 20, Size(640,480), true);
	}
    if(!cap.isOpened()){
        printf("Error: Could not open the camera!\n");
        return -1;
    }

	
	init();

	//main loop
	
    Mat frame, hframe;
	int key = 0;
    while(!(key==32 || key==27 || key==13)){
		double Times = (double)getTickCount();
       
		cap >> frame;
		//resize(frame, frame, Size(640,480));
		if(VideoFlag){
			Mat rmat = getRotationMatrix2D(Point2f(frame.cols / 2, frame.rows / 2), 180, 1);
			warpAffine(frame, frame, rmat, frame.size());  
		}
		

		std::vector<cv::Vec4i> lines;
		getHoughSil(lines, frame, 0.1, 0.2);
		//face detect		
		vector<FaceResult> faces;
		faceDetect(frame, faces);

		if(!faces.empty())
		{
			giveFacePositionAdvice(faces[0], frame);
			giveFaceOrientationAdvice(faces[0], frame);
		}
		
		//  1 blue front
		//  2 green left
		//  3 red   right
		HLD(frame);
			
		double slope = getHorizonSlope();
		//printf("Horizon slope: %lf\n", slope);

		//show image
		processFace(frame, faces);
		
		drawHorizonOrient(slope, frame);
		//hld.draw_ori(frame,  1.0 / hldfactor);
		
		drawFps(frame, Times);
		imshow(windowName, frame);
		//processWface(frame, faces);
		outputVideo << frame;
		key=waitKey(1);
    }
}


/*
CvRect faceDetectSingle(Mat Oframe){
	CvRect face(-1,-1,0,0);
	vector<Rect> faces;
	vector<int>	weights;
	Mat frame;
	double factor = 1.0;
	resize(Oframe, frame, Size(), factor, factor, CV_INTER_LINEAR);

	cc[0].detectMultiScale(frame, faces, weights, 1.05, 7, 0);
	int n = faces.size();
	int maxval = -1;
	int id = -1;
	for(int i = 0; i < n; ++i){
		if( weights[i] > maxval){
			maxval = weights[i];
			id = i;
		}
	}
	if( id != -1) 
		return CvRect(faces[id].x/factor,faces[id].y/factor,
   					  faces[id].width/factor,faces[id].height/factor);
		

	cc[1].detectMultiScale(frame, faces, weights, 1.05, 10, 0);
	n = faces.size();
	maxval = -1;
	id = -1;
	for(int i = 0; i < n; ++i){
		if( weights[i] > maxval){
			maxval = weights[i];
			id = i;
		}
	}
	if( id != -1) 
		return CvRect(faces[id].x/factor,faces[id].y/factor,
   					  faces[id].width/factor,faces[id].height/factor);

			

	Mat ff;
	flip(frame, ff, 1);
	cc[1].detectMultiScale(ff, faces, weights, 1.05, 10, 0);
	n = faces.size();
	maxval = -1;
	id = -1;
	for(int i = 0; i < n; ++i){
		if( weights[i] > maxval){
			maxval = weights[i];
			id = i;
		}
	}
	if( id != -1) {
		int X = ff.cols - faces[id].tl().x - faces[id].width - 1;
		int Y = faces[id].tl().y;
		return CvRect(X/factor, Y/factor, faces[id].width/factor, faces[id].height/factor);
	}

	return CvRect(-1,-1,0,0);
}
*/

/*
		//char imgname[100];
		//sprintf(imgname, "data/%d.jpg", index++);
		//imwrite(imgname, frame);
*/


