#include "clt_detect.hpp"
#include <vector>
#include <opencv2/opencv.hpp>
#include "utility.hpp"
#include "image_primitives.hpp"
#include "image_util.hpp"
#include "text_tools.hpp"
#include "tree_helper.hpp"
#include "rlt_detect.hpp"

using namespace std;
using namespace cv;
using namespace tree;

Rect expandRect(Rect r){
  int xd = r.width*0.15;
  int yd = r.height*0.15;
  return Rect(r.x-xd, r.y-yd, r.width + 2*xd, r.height + 2*yd);
}

vector<Rect> candidateCLT(ImageMeta im, vector<ComponentStats>& nonTextData)
{
  set<int> candidates;
  RT tree;
  for (int i = 0; i < nonTextData.size(); i++){
    if (isColorBlock(im, nonTextData[i])){
      insert2tree(tree, nonTextData[i].r, i);
      candidates.insert(i);      
    }
  }  
  vector<Rect> cltables;
  while (candidates.size()){
    int index = *candidates.begin();
    Rect r = nonTextData[index].r;
    while(true){
      Rect expanded = expandRect(r);
      vector<int> hits = search_tree(tree, expanded);
      if (hits.empty())
	break;
      for (int i = 0; i < hits.size(); i++){
	candidates.erase(hits[i]);
	remove_tree(tree, nonTextData[hits[i]].r, hits[i]);
	r |= nonTextData[hits[i]].r;
      }
    }
    cltables.push_back(r);
  }
  return cltables;
}

vector<Rect> findCLT(ImageDataBox& imd, ImageMeta& im){
  vector<ComponentStats>& nontextData = imd.nontextData;
  Mat& nontext = imd.nontext;
  Mat& text = imd.text;
  vector<Rect> candidates = candidateCLT(im, nontextData), tables;
  for (int i = 0; i < candidates.size(); i++){
    Mat region = nontext(candidates[i]);
    vector<vector<Point> > contours;
    findContours(region, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    vector<Point> flatContour;
    for (int j = 0; j < contours.size(); j++){
      flatContour.insert(flatContour.end(), contours[j].begin(), contours[j].end());
    }
    vector<Point> hull;
    convexHull(flatContour, hull);
    double angle = counterRotAngle(hull);
    Mat skewedRegion = text(candidates[i]), correctedRegion;
    rotate(skewedRegion, correctedRegion, angle);
    ComponentStats cs(candidates[i], 1, i);
    if (verifyRLT(correctedRegion, im, imd.textData, cs))
      tables.push_back(candidates[i]);
  }
  return tables;
}
