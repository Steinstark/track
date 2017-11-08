#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

vector<Rect> detectLetters(Mat img)
{
  vector<Rect> boundRect;
  Mat img_gray, img_sobel, img_threshold, element;
  cvtColor(img, img_gray, CV_BGR2GRAY);
  Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, BORDER_DEFAULT);
  threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
  element = getStructuringElement(MORPH_RECT, Size(17,3));
  morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element);
  vector<vector< Point> > contours;
  findContours(img_threshold, contours, 0, 1);
  vector<vector<Point> > contours_poly( contours.size() );
  for (int i = 0; i < contours.size(); i++){
    if (contours[i].size()>100){
      approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
      Rect appRect( boundingRect(Mat(contours_poly[i])));
      if (appRect.width>appRect.height)
	boundRect.push_back(appRect);
    }
  }
  return boundRect;
}


int main(){
  string filename("file0.png");
  Mat img = imread(filename.c_str());
  vector<Rect> lbb = detectLetters(img);
  for (int i = 0; i < lbb.size(); i++){
    rectangle(img, lbb[i], Scalar(0,255,0),3,8,0);
  }
  imshow("Letterboxes", img);
  waitKey(0);
  return 0;
}
