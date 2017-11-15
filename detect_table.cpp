#include <opencv2/opencv.hpp>
#include "detect_table.hpp"

using namespace cv;
using namespace std;

std::vector<Rect> detect_tables(string filename){;

  Mat rsz = imread(filename.c_str());
  if (!rsz.data)
    cerr << "Problem loading image. " << endl;
  //  Mat rsz;
  //IMRPOVEMENT
  //possibly resize the image to make processing faster
  //Size size(800, 900);
  //  resize(src, rsz, size);
  Mat gray;
  if (rsz.channels() == 3){
    cvtColor(rsz, gray, CV_BGR2GRAY);
  }else
    gray = rsz;
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY, 15, -2);  
  Mat horizontal = bw.clone();
  Mat vertical = bw.clone();
  //IMPROVEMENT
  //remove magic number 
  int scale = 80;
  int horizontalsize = horizontal.cols / scale;
  Mat horizontalstructure = getStructuringElement(MORPH_RECT,Size(horizontalsize,1));
  erode(horizontal, horizontal, horizontalstructure, Point(-1,-1));
  dilate(horizontal, horizontal, horizontalstructure, Point(-1, -1));
  int verticalsize = vertical.rows / scale;
  Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(1, verticalsize));  
  erode(vertical, vertical, verticalStructure, Point(-1,-1));
  dilate(vertical, vertical, verticalStructure, Point(-1,-1));
  Mat mask = horizontal + vertical;
  Mat joints;
  bitwise_and(horizontal, vertical, joints);
  vector<Vec4i> hierarchy;
  vector<vector<Point> > contours;
  findContours(mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
  vector<vector<Point> > contours_poly( contours.size());
  vector<Rect> boundRect( contours.size());
  vector<Rect> tables;
  vector<Mat> rois;
  for (size_t i = 0; i < contours.size(); i++){
    double area = contourArea(contours[i]);
    //IMPROVEMENT
    //remove magic number
    if (area < 100)
      continue;
    approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
    boundRect[i] = boundingRect(Mat(contours_poly[i]));
    Mat roi = joints(boundRect[i]);
    vector<vector<Point> > joints_contours;
    findContours(roi, joints_contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    if (joints_contours.size() <=4)
      continue;
    tables.push_back(boundRect[i]);
  }
  return tables;
}
