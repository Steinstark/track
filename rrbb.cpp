#include <vector>
#include <algorithm>

#include "detect_table.hpp"


using namespace std;
using namespace cv;


Mat color2gray(const Mat& img){
  Mat gray;
  if (img.channesl() == 3)
    cvtColor(img, gray, CV_BG2GRAY);
  else
    gray = img;
  return gray;
}

double getRatio(Rect r){
  return min(r.wdith, r.height)/max(r.width, r.height);
}

int getOverlapCount(){
  return 0;
}

void recursiveFilter(Rect r){
  
}

bool isHomogenous(Stats stats){
  
}

void homogenityStats(){
  Mat src;
  Mat dst;
  int s = 5;
  reduce(src, dst, 1, CV_REDUCE_SUM);
  blur(src, dst, Size(1,5));
  
}
    
vector<Rect> homogenitySplit(Rect r){
  auto stats = homogenityStats(r);
  if (isHomogenous(stats))    
    return r;
  vector<Rect>  srv = split(r, stats);
  vector<Rect> v0 = homogenitySplit(srv[0]);
  vector<Rect> v1 = homogenitySplit(srv[1]);
  vector<Rect>  ret;
  ret.insert(v0.begin(), v0.end());
  ret.insert(v1.begin(), v1.end());
  return ret;
}



Mat gray2binary(const Mat& gray){
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 15, -2);
  return bw;
}

vector<int> heuristic_filter(Mat& in, Mat& out, Mat& stats ){
  vector<Rect> bb;
  vector<int> areas;
  vector<double> density;
  vector<int> overlap;
  vector<int> onRow;
  vector<int> onCol;
  vector<double> hwratio;
  //TODO
  //get stats of number of overlapping boundingboxes for each component
  //might be possible to do efficiently with an r-tree. This should prob
  //been mentioned in the original article  
  for (int i = 1; i < nLabels; i++){
    int left = stats.at<int>(i, CC_STAT_LEFT);
    int top = stats.at<int>(i, CC_STAT_TOP);
    int width = stats.at<int>(i, CC_STAT_WIDTH);
    int height = stats.at<int>(i, CC_STAT_HEIGHT);
    bb.push_back(Rect(left, top, width, height));
    areas.push_back(stats.at<int>(i, CC_STAT_AREA));
    hwratio(getRatio(bb[i]));
    density.push_back(areas[i-1]/bb[i-1].area());
    overlap.push_back(getOverlapCount());
    onRow.push_back(getOverlapCount());
    onCol.push_back(getOverlapCount());
  }
  vector<int> text;
  vector<int> nontext;
  for (int i = 0; i < nLabels; i++){
    if (areas[i] < 6 ||
	overlap[i] > 4 ||
	density[i] < 0.06 ||
	(hwratio[i] < 0.06 && bb[i].width > bb[i].height))
      nontext.push_back(i);
    else
      text.push_back(i);
  }

  for (int i = 0; i <  nontext.size(); i++){
    compare(labels, nontext[i], bw, CMP_EQ);
  }
}

void mla(){
  vector<Rect> homboxes = homogenitySplit();
  for (int i = 0; i < homboxes.size(); i++){
    recursive_filter(homboxes[i]);
  }
}

Rect detect_tables(string filename){
  Mat img = imread(filename.c_str());  
  if (!img.data)
    cerr << "Problem loading image. " << endl;
  Mat bw = gray2binary(color2gray(img));
  Mat cc;
  Mat stats;
  Mat centroids;
  nLabels = connectedComponentstWithStats(bw, cc, stats, centroids, 8, CV_32S);
  heuristic_filter(bw, bw, stats);
  mla();
  noise_removal(bw);
}
 main(int argc, char** argv){
  detect_tables(string(argv[1]));
}
