#include "detect.hpp"
#include <vector>
#include <opencv2/opencv.hpp>
#include "utility.hpp"
#include "image_primitives.hpp"

using namespace std;
using namespace cv;

bool candidateRLT(ImageMeta& im, ComponentStats& cs){
  return isHorizontalLine(im, cs) ||
    regionIsRectangle(cs) ||
    isColorBlock(im, cs) ||
    containsManyTextElements(im, cs);
}

bool verifyRLT(Mat& text, ImageMeta& im, ComponentStats& cs){
  Mat region(text(cs.r));
  vector<TextLine> tls;
  return hasLowDensity(cs) &&
    regionIsRectangle(cs) &&
    manySmallRect(region, cs) &&
    noCut(im, cs.r) &&
    onlyText(im, cs.r) &&
    verticalArrangement(region, tls);  
}

vector<int> findRLT(Mat& text, vector<ComponentStats>& data, ImageMeta& im){
  vector<int> candidates;
  for (int i = 0; i < data.size(); i++){
    if (candidateRLT(im, data[i])){
      candidates.push_back(i);
    }
  }
  //skewcorrection
  vector<int> verified;
  for (int i = 0; i < candidates.size(); i++){
    if (verifyRLT(text, im, data[candidates[i]])){
      verified.push_back(candidates[i]);
    }
  }
  return verified;
}

vector<Rect> detect(Mat& text, Mat& nontext){
  vector<ComponentStats> textData = statistics(nontext);
  vector<ComponentStats> nontextData = statistics(nontext);
  ImageMeta meta(text.cols, text.rows, textData, nontextData);
  vector<int> rlt = findRLT(text, nontextData, meta);
  vector<Rect> tables;
  for (int i = 0; i < rlt.size(); i++){
    tables.push_back(nontextData[rlt[i]].r);
  }
  //nrlt_find(tables);
  return tables;
}
