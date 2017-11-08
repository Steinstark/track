#include <iostream>
#include <opencv2/opencv.hpp>
#include <sstream>

using namespace std;
using namespace cv;

int main(){
  const string filename("out-1.png");
  Mat src = imread(filename);
  if (!src.data)
    cerr << "Problem loading image. " << endl;
  Mat rsz;
  Size size(800, 900);
  resize(src, rsz, size);
  Mat gray;
  if (rsz.channels() == 3){
    cvtColor(rsz, gray, CV_BGR2GRAY);
  }else
    gray = rsz;
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY, 15, -2);  
  imshow("binary", bw);
  waitKey(0);
  Mat horizontal = bw.clone();
  Mat vertical = bw.clone();

  int scale = 80;
  int horizontalsize = horizontal.cols / scale;
  Mat horizontalstructure = getStructuringElement(MORPH_RECT,Size(horizontalsize,1));
  erode(horizontal, horizontal, horizontalstructure, Point(-1,-1));
  dilate(horizontal, horizontal, horizontalstructure, Point(-1, -1));
  imshow("horizontal", horizontal);
  waitKey(0);

  int verticalsize = vertical.rows / scale;
  Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(1, verticalsize));
  
  erode(vertical, vertical, verticalStructure, Point(-1,-1));
  dilate(vertical, vertical, verticalStructure, Point(-1,-1));
  imshow("vertical", vertical);
  waitKey(0);
  Mat mask = horizontal + vertical;
  imshow("mask",  mask);
  waitKey(0);
  Mat joints;
  bitwise_and(horizontal, vertical, joints);
  imshow("joints", joints);
  waitKey(0);

  vector<Vec4i> hierarchy;
  vector<vector<Point> > contours;
  findContours(mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
  vector<vector<Point> > contours_poly( contours.size());
  vector<Rect> boundRect( contours.size());
  vector<Mat> rois;
  for (size_t i = 0; i < contours.size(); i++){
    double area = contourArea(contours[i]);
    if (area < 100)
      continue;
    approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
    boundRect[i] = boundingRect(Mat(contours_poly[i]));
    Mat roi = joints(boundRect[i]);
    vector<vector<Point> > joints_contours;
    findContours(roi, joints_contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    if (joints_contours.size() <=4)
      continue;
    rois.push_back(rsz(boundRect[i]).clone());
    rectangle( rsz, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 1, 8, 0 );
  }
  for (size_t i = 0; i < rois.size(); i++){
    imshow("roi", rois[i]);
    stringstream ss;
    ss<< "file" << i << ".png";
    imwrite(ss.str().c_str(), rois[i]);
    waitKey();
  }
  imshow("contours", rsz);
  waitKey(0);
  return 0;  
}
