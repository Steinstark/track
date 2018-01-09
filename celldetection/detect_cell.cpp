#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include "detect_cell.hpp"

using namespace std;
using namespace cv;

std::vector<Rect> detect_cells(const Mat& bw, Rect r){
  Mat tableImage = bw(r);
  //IMPROVEMENT
  //remove magic number
  Mat mask;
  Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(10,1));
  Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(1,3));  
  dilate(tableImage, mask, horizontalStructure, Point(-1,-1));
  erode(mask, mask, verticalStructure, Point(-1,-1));
  
  //imshow("img", mask);
  //waitKey(0);  
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  findContours(mask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0,0));
  vector<vector<Point> > contours_poly(contours.size());
  vector<Rect> boundRect( contours.size());

  for (size_t i = 0; i < contours.size(); i++){
    approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true);
    boundRect[i] = boundingRect( Mat(contours_poly[i])) + r.tl();
  }
  /*Mat drawing = Mat::zeros(mask.size(), CV_8UC3 );
  RNG rng(12345);
  for (size_t i = 0; i < contours.size(); i++){
    Scalar color = Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,22));
    drawContours(drawing,contours_poly, (int)i, color, 1,8, vector<Vec4i>(), 0, Point() );
    rectangle(tableImage, boundRect[i].tl(), boundRect[i].br(), color, 2,8,0);
  }
  imshow("img", tableImage);
  waitKey(0);*/
  return boundRect;
}
