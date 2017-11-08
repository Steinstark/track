#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <string>

using namespace cv;
using namespace std;

int main(){
  Mat dst, cdst, cdstP;
  string file = "scan-hund.pdf-1_1.png";
  Mat src = imread(file.c_str(), IMREAD_GRAYSCALE);
  Canny(src, dst, 50, 200, 3);
  cvtColor(dst, cdst, COLOR_GRAY2BGR);
  cdstP = cdst.clone();
  vector<Vec2f> lines;
  HoughLines(dst, lines, 1, CV_PI/180, 150, 0, 0);
  for (size_t i = 0; i < lines.size(); i++){
    float rho = lines[i][0], theta = lines[i][1];
    Point pt1, pt2;
    double a = cos(theta), b = sin(theta);
    double x0 = a*rho, y0 = b*rho;
    pt1.x = cvRound(x0+1000*(-b));
    pt1.y = cvRound(y0+1000*(a));
    pt2.x = cvRound(x0-1000*(-b));
    pt2.y = cvRound(y0-1000*(a));
    line(cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
  }
  vector<Vec4i> linesP;
  HoughLinesP(dst, linesP, 1, CV_PI/180, 50, 50, 10);
  
  for( size_t i = 0; i < linesP.size(); i++){
    Vec4i l = linesP[i];
    line(cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
  }
  //imshow("Source", src);
  //  imshow("Detected Lines (in red) - Standard Hough Line Transform", cdst);
  imshow("Detected Lines (in red) - Probabilistic Line Transform", cdstP);
  waitKey(0);

  return 0;
}
