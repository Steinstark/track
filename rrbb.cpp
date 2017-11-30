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

void recursiveFilter(Rect r){
  
}
/*
bool isHomogenous(Stats stats){
  
}
*/

void homogenityStats(){
  Mat src;
  Mat dst;
  int s = 5;
  reduce(src, dst, 1, CV_REDUCE_SUM);
  blur(src, dst, Size(1,5));
  
}
    
vector<Rect> homogenitySplit(Rect r){
  vector<Rect> ret;
  //auto stats = homogenityStats(r);
  //  if (isHomogenous(stats))    
  //  return r;
  //  vector<Rect>  srv = split(r, stats);
  //vector<Rect> v0 = homogenitySplit(srv[0]);
  //vector<Rect> v1 = homogenitySplit(srv[1]);
  //vector<Rect>  ret;
  //ret.insert(v0.begin(), v0.end());
  //ret.insert(v1.begin(), v1.end());
  return ret;
}



Mat gray2binary(const Mat& gray){
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 15, -2);
  return bw;
}

vector<int> heuristic_filter(Mat& in, Mat& labels, Mat& stats ){
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
  imshow("mask", mask);
  waitKey(0);
  Mat res(in.size(), CV_8UC1, Scalar(0));
  in.copyTo(res, ~mask);
  imshow("Res", res);
  waitKey(0);
  return text;
}

void mla(){
  /* vector<Rect> homboxes = homogenitySplit();
  for (int i = 0; i < homboxes.size(); i++){
    recursive_filter(homboxes[i]);
    }*/
}

vector<Rect> detect_tables(string filename){
  Mat img = imread(filename.c_str(), IMREAD_GRAYSCALE);  
  if (!img.data)
    cerr << "Problem loading image. " << endl;
  Mat bw = gray2binary(img);
  Mat cc;
  Mat stats;
  Mat centroids;
  int nLabels = connectedComponentsWithStats(bw, cc, stats, centroids, 8, CV_32S);
  heuristic_filter(bw, cc, stats);
  //mla();
  //noise_removal(bw);
}

 main(int argc, char** argv){
  detect_tables(string(argv[1]));
}
