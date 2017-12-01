#include <vector>
#include <algorithm>

#include "detect_table.hpp"
#include "RTree.h"

using namespace std;
using namespace cv;

double getRatio(Rect r){
  return (double)min(r.width, r.height)/max(r.width, r.height);
}

void getOverlapCount(const vector<Rect>& bb,
			    vector<int>& overlap,
			    vector<int>& onRow,
			    vector<int>& onCol){
  const int inf = 1000000;
  RTree<int, int, 2, float> tree;
  for (int i = 0; i < bb.size(); i++){
    int tl[] = {bb[i].x, bb[i].y};
    int br[] = {bb[i].br().x, bb[i].br().y};
    tree.Insert(tl, br, i);
  }
  for (int i = 0; i < bb.size(); i++){
    int tl[] = {bb[i].x, bb[i].y};
    int br[] = {bb[i].br().x, bb[i].br().y};
    int count = tree.Search(tl, br, NULL, NULL);
    overlap.push_back(count-1);
    tl[0] = -inf;
    br[0] = inf;
    count = tree.Search(tl,br, NULL, NULL);
    onRow.push_back(count-1);
    tl[0] = bb[i].x;
    tl[1] = -inf;
    br[0] = bb[i].br().x;
    br[1] = inf;
    count = tree.Search(tl, br, NULL, NULL);
    onCol.push_back(count-1);    
  }
}

//TODO
//implement
void recursiveFilter(Rect r){
  return;
}

bool isExtrema(double a, double b){
  return (a < 0 && b > 0) || (a > 0 && b < 0);
}

double homogenity_stats(Mat& bw, Mat& sob, Mat& delta){
  Mat red;
  int s = 5;
  //histogram
  int scale = 255;
  reduce(bw, red, 1, CV_REDUCE_SUM, CV_64F);
  Mat sred = red / scale;  
  Mat blurred;
  //medelkernel 
  blur(sred, blurred, Size(1,30));  
  //gradienten
  Scharr(blurred, sob, -1, 0, 1);
  vector<int> a0;  
  for (int i = 1; i < sob.rows ; i++){
    if (isExtrema(sob.at<double>(i,0), sob.at<double>(i-1,0))){
      a0.push_back(i);
    }
  }  
  Mat peaks(a0.size(), 1, CV_32S, a0.data());
  vector<int> a1;
  for (int i = 1; i < a0.size(); i++){
    a1.push_back(a0[i] - a0[i-1]);
  }
  delta = Mat(a0.size()-1, 1, CV_32S, a1.data());
  Mat mean, stddev;
  meanStdDev(delta, mean, stddev);
  double st = stddev.at<double>(0,0);
  return st*st;
}

//TODO
//implement
void split_block(const vector<int>& pos, const vector<int>& neg){
  sort(pos.begin(), pos.end());
  sort(neg.begin(), neg.end());
  maxp = pos[pos.size()-1];
  maxn = neg[neg.size()-1];
  medianPos = median(pos);
  medianNeg = median(neg);
  int splitPos;
  if (maxp > medianPos)
    ;
  else if (maxn > medianNeg)
    ;
  else
    ;//EXPLODE

  
  return;
}

//TODO
//implement
void recursive_filter(){
  return;
}

vector<Rect> homogenous_regions(const Mat& img){
  queue<Rect> q;
  double varmax = 1.2;
  q.push(Rect(0,0, img.cols, img.rows));
  while(!q.empty()){
    Rect r = q.front();
    q.pop();
    Mat block = img(r), grad, delta;    
    if (homogenity_stats(block, grad, delta) > varmax)
      split_block();
  }

}



Mat gray2binary(const Mat& gray){
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 15, -2);
  return bw;
}

vector<int> heuristic_filter(Mat& in, Mat& labels, Mat& stats, Mat& textimg, Mat& nontextimg){
  int nLabels = stats.rows;
  vector<Rect> bb(nLabels);
  bb[0] = Rect(-1, -1, 0, 0);
  vector<int> areas(nLabels);
  vector<double> density(nLabels);
  vector<double> hwratio(nLabels);
  for (int i = 1; i < nLabels; i++){
    int left = stats.at<int>(i, CC_STAT_LEFT);
    int top = stats.at<int>(i, CC_STAT_TOP);
    int width = stats.at<int>(i, CC_STAT_WIDTH);
    int height = stats.at<int>(i, CC_STAT_HEIGHT);
    bb[i] = Rect(left, top, width, height);
    areas[i] = stats.at<int>(i, CC_STAT_AREA);
    hwratio[i] = getRatio(bb[i]);
    density[i] = (double)areas[i]/bb[i].area();    
  }
  vector<int> onRow;
  vector<int> onCol;
  vector<int> overlap;
  getOverlapCount(bb, overlap, onRow, onCol);

  vector<int> text;
  vector<int> nontext;
  for (int i = 1; i < nLabels; i++){
    if (areas[i] < 6 ||
	overlap[i] > 4 ||
	(hwratio[i] < 0.06 && bb[i].width > bb[i].height) ||      
	density[i] < 0.06){
      nontext.push_back(i);
    }
    else
      text.push_back(i);
  }
  Mat mask(labels.size(), CV_8UC1, Scalar(0));
  for (int i = 0; i <  nontext.size(); i++){
    mask = mask | (labels==nontext[i]);
  }
  textimg = Mat(in.size(), CV_8UC1, Scalar(0));
  in.copyTo(textimg, ~mask);
  for (int i = 0; i <  text.size(); i++){
    mask = mask | (labels== text[i]);
  }
  nontextimg = Mat(in.size(), CV_8UC1, Scalar(0));
  in.copyTo(nontextimg, ~mask);
  imshow("text", text);
  waitKey(0);
  return text;
}

void mla(const Mat& img){
  vector<Rect> homboxes = homogenous_regions(img);
  for (int i = 0; i < homboxes.size(); i++){
    //recursive_filter(homboxes[i]);
  }
}


//BUG
//invalid use of pointers. Needs fixing.
vector<Rect> detect_tables(string filename){
  Mat img = imread(filename.c_str(), IMREAD_GRAYSCALE);  
  if (!img.data)
    cerr << "Problem loading image. " << endl;
  Mat bw = gray2binary(img);
  Mat cc;
  Mat stats;
  Mat centroids;
  int nLabels = connectedComponentsWithStats(bw, cc, stats, centroids, 8, CV_32S);
  Mat text, nontext;
  heuristic_filter(bw, cc, stats,text, nontext);
  mla(text);
  //noise_removal(bw);
}

 main(int argc, char** argv){
  detect_tables(string(argv[1]));
}
