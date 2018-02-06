#include "detect.hpp"
#include <vector>
#include <opencv2/opencv.hpp>
#include "utility.hpp"
#include "image_primitives.hpp"
#include "image_util.hpp"
#include "text_tools.hpp"
#include "tree_helper.hpp"

using namespace std;
using namespace cv;
using namespace tree;

ImageDataBox::ImageDataBox(Mat& text, Mat& nontext): text(text), nontext(nontext){
  textData = statistics(text);
  nontextData = statistics(nontext);
}

bool candidateRLT(ImageMeta& im, ComponentStats& cs){
  return
    containsManyTextElements(im, cs) &&
    (isHorizontalLine(im, cs) ||
     regionIsRectangle(cs) ||
     isColorBlock(im, cs));
}

bool verifyRLT(Mat& region,
	       ImageMeta& im,
	       vector<ComponentStats>& textData,
	       ComponentStats& cs)
{
  if (hasLowDensity(cs) &&
      regionIsRectangle(cs) &&
      manySmallRect(region, cs) &&
      noCut(im, textData, cs.r)){
    vector<TextLine> tls = linesInRegion(im, textData, cs.r);
    Mat tableCopy = region.clone();
    for (int i = 0; i < tls.size(); i++){
      rectangle(tableCopy, tls[i].getBox()-cs.r.tl(), Scalar(255), CV_FILLED);
    }
    return verticalArrangement(tableCopy, tls);  
  }
  return false;
}

vector<Rect> findRLT(ImageDataBox& imd, ImageMeta& im)
{
  vector<int> candidates;
  vector<ComponentStats>& nontextData = imd.nontextData;
  vector<ComponentStats>& textData = imd.textData;
  Mat& text = imd.text;
  for (int i = 0; i < nontextData.size(); i++){
    if (candidateRLT(im, nontextData[i])){
      candidates.push_back(i);
    }
  }
  vector<Rect> verified;
  for (int i = 0; i < candidates.size(); i++){
    Mat skewedRegion = text(nontextData[candidates[i]].r), correctedRegion;
    double angle = counterRotAngle(skewedRegion);
    rotate(skewedRegion, correctedRegion, angle);
    if (verifyRLT(correctedRegion, im, textData, nontextData[candidates[i]])){
      verified.push_back(nontextData[candidates[i]].r);
    }
  }
  return verified;
}

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

vector<Rect> mergeIntersecting(vector<Rect>& tables){
  set<int> toVisit;
  vector<Rect> merged;
  RT tree;
  for (int i = 0; i < tables.size(); i++){
    toVisit.insert(i);
    insert2tree(tree, tables[i], i);
  }
  while (toVisit.size()){
     int index = *toVisit.begin();
     Rect r = tables[index];
     while (true){
       vector<int> hits = search_tree(tree, r);
       if (hits.empty())
	 break;
       for (int i = 0; i < hits.size(); i++){
	 r |= tables[hits[i]];
	 toVisit.erase(hits[i]);
	 remove_tree(tree, tables[hits[i]], hits[i]);
       }
     }     
     merged.push_back(r);
  }
  return merged;
}

void findNRLT(Mat& text, vector<ComponentStats>& data, ImageMeta& im){
  
}

vector<Rect> detect(Mat& text, Mat& nontext){
  ImageDataBox imd(text, nontext);
  ImageMeta im(text.cols, text.rows, imd.textData, imd.nontextData);
  vector<Rect> tables = findRLT(imd, im);  
  vector<Rect> colorTables = findCLT(imd, im); //even if it does work
  return mergeIntersecting(tables);
}
