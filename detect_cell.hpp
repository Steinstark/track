#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

std::vector<cv::Rect> detect_cells(std::string filename, cv::Rect r){
  using namespace std;
  using namespace cv;
  Mat img = imread(filename.c_str());
  Mat rsz = Mat(img, r);
  imshow("rsz", rsz);
  waitKey(0);
  if (!rsz.data)
    cerr << "Problem loading image. " << endl;
  //IMPROVEMENT
  //Resize image to make processing faster
  //Mat rsz;  
  //Size size(800, 900);
  // resize(src, rsz, size);
  Mat gray;
  if (rsz.channels() == 3){
    cvtColor(rsz, gray, CV_BGR2GRAY);
  }else
    gray = rsz;
  Mat bw;
  imshow("gray", gray);
  waitKey(0);
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 15, -2);
  Mat horizontal = bw.clone();
  Mat vertical = bw.clone();
  //IMPROVEMENT
  //remove magic number
  int scale = 10;
  int horizontalsize = horizontal.cols / scale;
  Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize,1));  
  erode(horizontal, horizontal, horizontalStructure, Point(-1, -1));
  dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1));
  int verticalsize = vertical.rows / scale;  
  Mat verticalStructure = getStructuringElement(MORPH_RECT, Size( 1,verticalsize));  
  erode(vertical, vertical, verticalStructure, Point(-1, -1));
  dilate(vertical, vertical, verticalStructure, Point(-1, -1));
  horizontalStructure = getStructuringElement(MORPH_RECT, Size(1,3));
  verticalStructure = getStructuringElement(MORPH_RECT, Size(3,1));
  dilate(vertical, vertical, verticalStructure, Point(-1,-1));
  dilate(horizontal, horizontal, horizontalStructure, Point(-1,-1));
  
  Mat mask = horizontal + vertical;
  imshow("bin+mask", bw + mask);
  //  imshow("mask", mask);
  waitKey(0);
  Mat masked;  
  bw.copyTo(masked, ~mask);
  imshow("masked", masked);
  waitKey(0);
  //IMPROVEMENT
  //remove magic number
  int es=6;
  int ed=4;
  //IMPROVEMENT
  //remove magic number
  //Mat elem_er = getStructuringElement( cv::MORPH_ELLIPSE,Size( es + 1, 2*es+1 ),Point( es, es ) );
  //Mat elem_di = getStructuringElement( cv::MORPH_ELLIPSE,Size( 4*ed + 1, ed+1 ),Point( ed, ed ) );
  //IMPROVEMENT
  //remove magic number
  horizontalStructure = getStructuringElement(MORPH_RECT, Size(10,1));
  verticalStructure = getStructuringElement(MORPH_RECT, Size(1,3));
  dilate(masked, masked, horizontalStructure, Point(-1,-1));
	 erode(masked, masked, verticalStructure, Point(-1,-1));
  verticalStructure = getStructuringElement(MORPH_RECT, Size(9, 5));
  dilate(masked, masked, verticalStructure, Point(-1,-1));
  //dilate(masked,masked,elem_di);
  //erode(masked,masked,elem_er);
  //  dilate(masked,masked,elem_di);
  imshow("blurred", masked);
  waitKey(0);  
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  findContours(masked, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0,0));
  vector<vector<Point> > contours_poly(contours.size());
  vector<Rect> boundRect( contours.size());

  for (size_t i = 0; i < contours.size(); i++){
    approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true);
    boundRect[i] = boundingRect( Mat(contours_poly[i]));
  }
  Mat drawing = Mat::zeros(masked.size(), CV_8UC3 );
  RNG rng(12345);
  for (size_t i = 0; i < contours.size(); i++){
    Scalar color = Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,22));
    drawContours(drawing,contours_poly, (int)i, color, 1,8, vector<Vec4i>(), 0, Point() );
    rectangle(rsz, boundRect[i].tl(), boundRect[i].br(), color, 2,8,0);
  }
  imshow("drawing", rsz);
  waitKey(0);
  return boundRect;
}
