#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "HorizonLineDetector.h"
#include <stdlib.h>
const static double PI = 3.1415926535897;

extern HorizonLineDetector hld;

double getHorizonSlope()
{
		double xsum=0;
		double ysum=0;
		double xsquaredsum=0;
		double xtimesysum=0;
		if( hld.horizon.size() != 0 )
		for(size_t i=0; i!=hld.horizon.size()-1; ++i)
		{
			//printf("%d %d ", hld.horizon[i].x, hld.horizon[i].y);
			xsum+=hld.horizon[i].x;
			ysum+=hld.horizon[i].y;
			xtimesysum+=hld.horizon[i].x*hld.horizon[i].y;
			xsquaredsum+=hld.horizon[i].x*hld.horizon[i].x;
		}
		//printf("\n");
		//printf("Horizon: %ld\n", hld.horizon.size());
		return (xtimesysum-xsum*ysum/hld.horizon.size())/(xsquaredsum-xsum*xsum/hld.horizon.size());
}

/*void getHoughSil(std::vector<cv::Vec2f> lines, cv::Mat& image)
{
	cv::Mat temp;
	cv::Canny(image, temp, 125, 350);

	cv::HoughLines(temp, lines, 1, PI/180, 60);//80?
	cv::Mat result(temp.rows, temp.cols, CV_8U, cv::Scalar(255));
	image.copyTo(result);

	for(size_t i=0; i!=lines.size(); ++i)
	{
		float rho = lines.at(i)[0];
		float theta = lines.at(i)[1];

		cv::Point pt1(rho/cos(theta), 0);
		cv::Point pt2(rho-result.rows*sin(theta)/cos(theta), result.rows);

		if(theta < PI/4 || theta > 3*PI/4)
		{
			cv::line(result, pt1, pt2, cv::Scalar(255), 1);
		}
		else
		{
			cv::line(result, pt1, pt2, cv::Scalar(255), 1);
		}
	}
	cv::imshow("Horizontal line", result);
}*/
void getHoughSil(std::vector<cv::Vec4i> lines, cv::Mat& srcImage, double lengthThresh, double slopeThresh)
{
	cv::Mat midImage, dstImage;
	cv::Canny(srcImage, midImage, 50.0, 200, 3);
	cvtColor(midImage, dstImage, CV_GRAY2BGR);

	HoughLinesP(midImage, lines, 1, CV_PI/180, 80 ,50, 10);

	for(size_t i=0; i!=lines.size(); ++i)
	{
		cv::Vec4i l = lines[i];
		double lengthPortion = (double)((l[2]-l[0])*(l[2]-l[0])+(l[3]-l[1])*(l[3]-l[1]))/(srcImage.cols*srcImage.cols+srcImage.rows*srcImage.rows);
		double slopeVertical = (double)(l[2] - l[0])/(l[3] - l[1]);
		double slopeHorizonal = (double)(l[3] - l[1])/(l[2] - l[0]);
		//printf("%lf %lf %lf\n", lengthPortion, slopeVertical, slopeHorizonal);

		if(lengthPortion >= lengthThresh && (abs(slopeVertical)<slopeThresh||abs(slopeHorizonal)<slopeThresh))
		{
			cv::line(dstImage, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(186, 88, 255), 1, CV_AA);
		}
	}

	//cv::imshow("Sils", dstImage);
}

void giveFacePositionAdvice(FaceResult& res, cv::Mat& img)
{
	int faceCenterx = res.face.x + res.face.width / 2;
	int faceCentery = res.face.y + res.face.height / 2;
	int w = img.cols;
	int h = img.rows;
	//printf("w, h, faceX, faceY\n%d, %d, %d, %d\n", w, h, faceCenterx, faceCentery);
	int portion = 4;
	if(faceCenterx <= w / portion)
	{
		putText(img, "Adjust to your left!", cv::Point(w / 2, h / 2), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(128, 0, 255), 2, 16);
		return;
	}
	if(w - faceCenterx <= w / portion)
	{
		putText(img, "Adjust to your right!", cv::Point(w / 2, h / 2), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(128, 0, 255), 2, 16);
		return;
	}
	if(faceCentery <= h / portion)
	{
		putText(img, "Lower your stance!", cv::Point(w / 2, h / 2), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(128, 0, 255), 2, 16);
		return;
	}
	if(h - faceCentery <= h / portion)
	{
		putText(img, "Higher your stance!", cv::Point(w / 2, h / 2), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(128, 0, 255), 2, 16);
		return;
	}
}

void giveFaceOrientationAdvice(FaceResult& res, cv::Mat &img)
{
	int faceCenterx = res.face.x + res.face.width / 2;
	int faceCentery = res.face.y + res.face.height / 2;
	int w = img.cols;
	int h = img.rows;
	//printf("w, h, faceX, faceY\n%d, %d, %d, %d\n", w, h, faceCenterx, faceCentery);
	int portion = 3;
	if(res.dir == 1)
		return;
	if(res.dir == 2 && faceCenterx < w / portion)
	{
		putText(img, "Look to your right!", cv::Point(0, h / 2), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(128, 0, 255), 2, 16);
		return;
	}
	if(res.dir == 3 && faceCenterx >= (portion - 1)*w / portion)
	{
		putText(img, "Look to your left!", cv::Point(0, h / 2), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(128, 0, 255), 2, 16);
		return;
	}
}

void drawHorizonOrient(double slope, cv::Mat &img)
{
	int h = img.rows;
	int w = img.cols;
	cv::Point pt1 = cv::Point(0, h / 2 - slope* w / 8);
	cv::Point pt2 = cv::Point(w, h / 2 + slope*w / 8);
	cv::line(img, pt1, pt2, cv::Scalar(0, 255, 0), 4);
	if(slope > 0.1 || slope < -0.1)
	{
		//printf("%lf\n", abs(slope));
		putText(img, "Camera is not horizontal!", cv::Point(0, h / 4), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 128, 255), 2, 16);
		return;
	}
}
