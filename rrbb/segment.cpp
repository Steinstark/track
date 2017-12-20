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
    ordered_non_unique<member<ComponentStats, double, &ComponentStats::density>, greater<int> >,
    ordered_non_unique<member<ComponentStats, double, &ComponentStats::hwratio>, greater<int> >,
    ordered_non_unique<member<ComponentStats, double, &ComponentStats::density>, less<int> >
    >
  >;

//v is assumed to be sorted
int getQ3(vector<Line> v){
  int n = v.size();
  int pos = 3*n/4;
  if (!n || !v[pos].length()){
    cout << "size or length is 0" << endl;
    return 1;    
  }
  return v[pos].length();
}

Mat text_segment(Mat& text){
  Mat cc, stats, centroids;
  int labels = connectedComponentsWithStats(text, cc, stats, centroids, 8, CV_32S);
  Mat bounds(text.size(), CV_8UC1, Scalar(0));
  for (int i = 1; i < labels; i++){
    Rect r = stats2rect(stats, i);
    rectangle(bounds, r, Scalar(255));
  }
  //  imshow("bounds", bounds);
  //  waitKey(0);
  vector<Rect> regions = homogenous_regions(bounds);
  //for (Rect r : regions){
  //rectangle(bounds, r, Scalar(255));
  //}
  //  imshow("homogen bounds", bounds);
  //  waitKey(0);
  Mat textBlob(text.size(), CV_8UC1, Scalar(0));
  for (Rect r : regions){
    int kernel[2];
    for (int i = 0; i < 2; i++){
      Mat hist;
      reduce(text(r), hist, i, CV_REDUCE_SUM, CV_64F);
      vector<Line> textLine, spaceLine;
      find_all_lines(hist, textLine, spaceLine);
      sort(spaceLine.begin(), spaceLine.end(), [](Line a, Line b){ return a.length() < b.length();});
      int val = getQ3(spaceLine);
      kernel[i] = val;
    }    
    Mat element = getStructuringElement(MORPH_RECT, Size(2*kernel[0], 2*kernel[1]), Point(-1, -1));
    dilate(bounds(r), bounds(r), element);
    erode(bounds(r), bounds(r), element);
  }
  return bounds;
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

/*
Mat table_segment(Mat& nontext, Mat& text, CompDB&  db, const Mat& cc, RT t_tree, RT nt_tree){
  auto&p = db.get<1>();
  auto it = p.begin();
  Mat tableBlob(cc.size(), CV_8UC1, Scalar(0));
  while (it != p.end() && it-> density <= 0.02){
    if (noCut(t_tree, it->r) &&
	noCut(nt_tree, it->r) &&
	manySmallRect(text(it->r)) &&
	sumAreaInside(t_tree, it->r) >= 0.99 &&
	areaInside / it->r.area >= 0.99 &&
	!search_tree(nt_tree, it->r)){
      move2(nontext, tableBlob, cc, it->index);
      it = p.erase(it);
    }else
      it++;    	
  }
  return tableBlob  
  }*/

Mat separator_segment(Mat& nontext, CompDB& db, const Mat& cc, RT& tree){
  auto&p = db.get<3>();
  auto it = p.begin();
  Mat separatorBlob(cc.size(), CV_8UC1, Scalar(0));
  while(it != p.end() && it->density <= 0.02){
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

void segment(Mat& text, Mat& nontext){
  Mat t_cc, t_stats, t_centroids;
  RT t_tree;
  CompDB db;
  int t_labels = connectedComponentsWithStats(text, t_cc, t_stats, t_centroids, 8, CV_32S);
  for (int i = 1; i < t_labels; i++){
    Rect r = stats2rect(t_stats, i);
    insert2tree(t_tree, r, i);
  }
  Mat nt_cc, nt_stats, nt_centroids;
  int nt_labels = connectedComponentsWithStats(nontext, nt_cc, nt_stats, nt_centroids, 8, CV_32S);
  for (int i = 1; i < nt_labels; i++){
    db.insert(stats2component(nt_stats, i));
  }
  Mat textBlob = text_segment(text);
  Mat lineBlob = line_segment(nontext, db, nt_cc);
  //Mat tableBlob = table_segment(nontext, text, db, nt_cc);
  Mat separatorBlob = separator_segment(nontext, db, nt_cc, t_tree);
  Mat imageBlob = image_segment(nontext, db, nt_cc);

  imshow("textBlob", textBlob);
  imshow("lineBlob", lineBlob);
  imshow("separatorBlob", separatorBlob);
  imshow("imageBlob", imageBlob);
  waitKey(0);  
}
