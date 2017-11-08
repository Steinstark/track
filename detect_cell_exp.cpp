#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace cv;

int main(){
  const string filename("file0.png");
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

  /*Mat dst, cdst;
  Canny(src, dst, 50, 200, 3);
  imshow("canny", dst);
  waitKey(0);
  cvtColor(dst, cdst, CV_GRAY2BGR);
  imshow("cvt", cdst);
  waitKey(0);
  vector<Vec2f> lines; 
  HoughLines(dst, lines, 1, CV_PI/180,0,0,1000000);
  for (size_t i = 0; i < lines.size(); i++){
    float rho = lines[i][0], theta = lines[i][1];
    Point pt1, pt2;
    double a = cos(theta), b = sin(theta);
    double x0 = a*rho, y0 = b*rho;
    pt1.x = cvRound(x0 + 1000*(-b));
    pt1.y = cvRound(y0 + 1000*(a));
    pt2.x = cvRound(x0 - 1000*(-b));
    pt2.y = cvRound(y0 - 1000*(a));
    line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
  }
  imshow("detected", cdst);
  waitKey(0);*/

  
  // Create the images that will use to extract the horizontal and vertical lines
  Mat horizontal = bw.clone();
  Mat vertical = bw.clone();  
  int horizontalsize = horizontal.cols / 30;  
  Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize,1));  
  erode(horizontal, horizontal, horizontalStructure, Point(-1, -1));
  dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1));    
  int verticalsize = vertical.rows / 30;  
  Mat verticalStructure = getStructuringElement(MORPH_RECT, Size( 1,verticalsize));  
  erode(vertical, vertical, verticalStructure, Point(-1, -1));
  dilate(vertical, vertical, verticalStructure, Point(-1, -1));  
  bitwise_not(vertical, vertical);
  Mat edges;
  adaptiveThreshold(vertical, edges, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, -2);
  Mat kernel = Mat::ones(2, 2, CV_8UC1);
  dilate(edges, edges, kernel);
  Mat smooth;
  vertical.copyTo(smooth);
  imshow("smooth",smooth);
  waitKey(0);
  blur(smooth, smooth, Size(2, 2));
  Mat masked, mask;
  bitwise_and(vertical,~horizontal,mask);
  bw.copyTo(masked, mask);
  imshow("Masked", masked);  
  waitKey(0);
  int es=6;
  int ed=4;
  Mat elem_er = getStructuringElement( cv::MORPH_ELLIPSE,Size( es + 1, 2*es+1 ),Point( es, es ) );
  Mat elem_di = getStructuringElement( cv::MORPH_ELLIPSE,Size( 4*ed + 1, ed+1 ),Point( ed, ed ) );  
  dilate(masked,masked,elem_di);
  erode(masked,masked,elem_er);
  dilate(masked,masked,elem_di);
  imshow("eroded", masked);
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
    rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2,8,0);
  }
  imshow("Contours", drawing);
  waitKey(0);
  return 0;
}
