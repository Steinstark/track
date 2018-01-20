#include "segment.hpp"
#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include "homogenous_regions.hpp"
#include "utility.hpp"
#include "RTree.h"
#include "tree_helper.hpp"

using namespace std;
using namespace cv;
using namespace boost::multi_index;

using RT = RTree<int, int, 2, float>;
using CompDB = multi_index_container<
  ComponentStats,
  indexed_by<
    ordered_non_unique<member<ComponentStats, int, &ComponentStats::area>, greater<int> >,
    ordered_non_unique<member<ComponentStats, double, &ComponentStats::density>, greater<double> >,
    ordered_non_unique<member<ComponentStats, double, &ComponentStats::hwratio>, greater<double> >,
    ordered_non_unique<member<ComponentStats, double, &ComponentStats::density>, less<double> >
    >
  >;

//v is assumed to be sorted
int getQ3(const vector<Line>& v){
  int n = v.size();
  int pos = 3*n/4;
  if (!n || !v[pos].length()){
    cout << "size or length is 0" << endl;
    return 15;    
  }
  return v[pos].length();
}

//TODO
//Find and fix errors of text blobbing and segmentation
Mat text_segment(Mat& text){
  vector<Rect> regions = homogenous_regions(text);
  Mat textBlob(text.size(), CV_8UC1, Scalar(0));
  for (Rect r : regions){
    Mat blob;
    findNonZero(text(r), blob);
    Rect br = boundingRect(blob);
    rectangle(textBlob, br+r.tl(), Scalar(255), CV_FILLED);
  }
  return textBlob;
}

Mat line_segment(Mat& nontext, CompDB& db, const Mat& cc){
  auto& p = db.get<1>();
  auto it = p.begin();
  Mat lineBlob(cc.size(), CV_8UC1, Scalar(0));
  while (it != p.end() && it->density >= 0.9){
    if (it->hwratio <= 0.1){
      move2(nontext,lineBlob, cc, it->index);
      it = p.erase(it);
    }else
      it++;
  }
  return lineBlob;
}

bool noCut(RT& tree, Rect r){
  Rect left(r.x-1, r.y, 1, r.height);
  Rect right(r.x+r.width, r.y, 1, r.height);
  Rect top(r.x, r.y-1, r.width, 1);
  Rect bottom(r.x, r.y+r.height, r.width, 1);
  return !search_tree(tree, left) &&
    !search_tree(tree, top) &&
    !search_tree(tree, right) &&
    !search_tree(tree, bottom);    
}

int  manySmallRect(Mat& img, const Rect& r, RT& tree){
  
  Mat rev =  ~img(r), cc, stats, centroids;
  int labels = connectedComponentsWithStats(rev, cc, stats, centroids, 8, CV_32S);
  int area = 0;
  for (int i = 1; i < labels; i++){
    ComponentStats cs = stats2component(stats, i);
    if (cs.density > 0.5 && search_tree(tree, cs.r + r.tl()))
      area += cs.area;
    else
      return 0;
  }
  return area;
}

Mat table_segment(Mat& nontext, Mat& text, CompDB&  db, const Mat& cc, RT& t_tree, RT& nt_tree){
  auto&p = db.get<3>();
  auto it = p.begin();
  Mat tableBlob(cc.size(), CV_8UC1, Scalar(0));
  while (it != p.end() && it-> density <= 0.2){
    if (noCut(t_tree, it->r) &&
	//search_tree(nt_tree, it->r) < 2 &&
	(double)manySmallRect(text, it->r, t_tree)/it->bb_area >= 0.8){    
      move2(nontext, tableBlob, cc, it->index);
      it = p.erase(it);
    }else
      it++;    	
  }
  return tableBlob;  
}

Mat separator_segment(Mat& nontext, CompDB& db, const Mat& cc, RT& tree){
  auto&p = db.get<3>();
  auto it = p.begin();
  Mat separatorBlob(cc.size(), CV_8UC1, Scalar(0));
  while(it != p.end() && it->density <= 0.2){
    if (search_tree(tree, it->r)){
      move2(nontext, separatorBlob, cc, it->index);
      it = p.erase(it);
    }else
      it++;
  }
  return separatorBlob;
}

Mat image_segment(Mat& nontext, CompDB& db, Mat& cc){
  Mat imageBlob(cc.size(), CV_8UC1, Scalar(0));
  auto& p = db.get<0>();
  auto it = p.begin();
  while (it != p.end()){
    move2(nontext, imageBlob, cc, it->index);
    it++;
  }
  return imageBlob;
}

Mat segment(Mat& text, Mat& nontext){
  Mat t_cc, t_stats, t_centroids;
  RT t_tree;
  CompDB db;
  int t_labels = connectedComponentsWithStats(text, t_cc, t_stats, t_centroids, 8, CV_32S);
  for (int i = 1; i < t_labels; i++){
    Rect r = stats2rect(t_stats, i);
    insert2tree(t_tree, r, i);
  }
  Mat nt_cc, nt_stats, nt_centroids;
  RT nt_tree;
  int nt_labels = connectedComponentsWithStats(nontext, nt_cc, nt_stats, nt_centroids, 8, CV_32S);
  for (int i = 1; i < nt_labels; i++){
    ComponentStats cs = stats2component(nt_stats, i);
    db.insert(cs);
    insert2tree(nt_tree, cs.r, i);
  }
  Mat textBlob = text_segment(text);
  Mat lineBlob = line_segment(nontext, db, nt_cc);
  Mat tableBlob = table_segment(nontext, text, db, nt_cc, t_tree, nt_tree);
  Mat separatorBlob = separator_segment(nontext, db, nt_cc, t_tree);
  separatorBlob = separatorBlob | lineBlob;
  Mat imageBlob = image_segment(nontext, db, nt_cc);
  /*  imshow("text", text);
  imshow("tableBlob", tableBlob);
  imshow("separatorBlob", separatorBlob);
  imshow("imageBlob", imageBlob);
  waitKey(0);*/
  return tableBlob;
}


