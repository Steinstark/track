#include <vector>
#include <opencv2/opencv.hpp>
#include "detect_cell.hpp"

using namespace std;
using namespace cv;

//IMPROVEMENT
//remove magic number for size of dilate
vector<Rect> detect_cells(const Mat& bw, Rect r){
  Mat tableImage = bw(r);
  Mat mask;
  Mat element = getStructuringElement(MORPH_RECT, Size(20,5));
  dilate(tableImage, mask, element, Point(-1,-1));  
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
  /*Mat drawing = bw.clone();
  Scalar color = Scalar(255);
  for (size_t i = 0; i < contours.size(); i++){
    //    drawContours(drawing,contours_poly, (int)i, color, 1,8, vector<Vec4i>(), 0, Point() );
    rectangle(drawing, boundRect[i], color);
    }*/
  //imshow("img", drawing);
  //  waitKey(0);
  return boundRect;
}
