#include "utility.hpp"
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

ComponentStats::ComponentStats(Rect r, int area, int i) : r(r), area(area),  index(i){
  bb_area = r.area();
  density = (double)area/bb_area;
  hwratio = (double)min(r.width, r.height)/max(r.width, r.height);
}

void move2(Mat& from, Mat& to, const Mat& cc, int i){
  Mat mask(cc.size(), CV_8UC1, Scalar(0));
  mask = mask | (cc==i);
  mask = mask & from;
  to = to | mask;
  from = from & ~mask;
}

Rect stats2rect(const Mat& stats, int i){
  int left = stats.at<int>(i, CC_STAT_LEFT);
  int top = stats.at<int>(i, CC_STAT_TOP);
  int width = stats.at<int>(i, CC_STAT_WIDTH);
  int height = stats.at<int>(i, CC_STAT_HEIGHT);
  return Rect(left, top, width, height);
}

ComponentStats stats2component(const Mat& stats, int i){
  Rect r = stats2rect(stats, i);
  int area = stats.at<int>(i, CC_STAT_AREA);
  return ComponentStats(r, area, i);
}

void displayHist(string str, const Mat& img){
  int m = 0;
  int length = max(img.rows, img.cols);
  for (int i = 0; i < length; i++){
    int val = img.at<double>(i);
    m = max(m,val);
  }
  Mat hist;
  if (img.rows > img.cols){
    hist = Mat::zeros(img.rows, m+50, CV_8U);
    for (int i = 0; i < img.rows; i++){
      for (int j = 0; j < img.at<double>(i,0); j++){
	hist.at<uchar>(i,j) = 255;
      }
    }
  }else{
    hist = Mat::zeros(m+50, img.cols, CV_8U);
    for (int i = 0; i < img.cols; i++){
      for (int j = 0; j < img.at<double>(0, i); j++){
	hist.at<uchar>(j,i) = 255;
      }
    }
  }
  imshow(str, hist);
}

void find_lines(const Mat& hist, vector<Line>& text, vector<Line>& space){
  int length = max(hist.rows, hist.cols);
  int t = 0;
  for (int i = 1; i < length; i++){
    if (!hist.at<double>(i-1) && hist.at<double>(i)){
      t = i;
    }
    else if (hist.at<double>(i-1) && !hist.at<double>(i)){
      text.push_back(Line(t,i-1));
      t = -1;
    }
  }
  if (t != -1)
    text.push_back(Line(t,length-1));
  for (int i = 1; i < text.size(); i++){
    space.push_back(Line(text[i-1].r+1, text[i].l-1));
  }
}

void find_all_lines(const Mat& hist, vector<Line>& text, vector<Line>& space){
  find_lines(hist, text, space);
  Line a = text.front();
  Line b = text.back();
  int length = max(hist.rows, hist.cols);
  if (a.l > 1)
    space.push_back(Line(0, a.l-1));
  if (b.r < length-1)
    space.push_back(Line(b.r, length-1));      
}

