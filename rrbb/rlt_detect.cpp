#include "rlt_detect.hpp"
#include <list>
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
      rectangle(tableCopy, tls[i].getBox(), Scalar(255), CV_FILLED);
    }
    return verticalArrangement(tableCopy, tls);  
  }
  return false;
}

bool candidateRLT(ImageMeta& im, ComponentStats& cs){
  return
    containsManyTextElements(im, cs) &&
    (isHorizontalLine(im, cs) ||
     regionIsRectangle(cs) ||
     isColorBlock(im, cs));
}

list<Rect> findRLT(ImageDataBox& imd, ImageMeta& im)
{
  list<ComponentStats> candidates;
  vector<ComponentStats>& nontextData = imd.nontextData;
  vector<ComponentStats>& textData = imd.textData;
  Mat& text = imd.text;
  for (ComponentStats& cs: nontextData){
    if (candidateRLT(im, cs)){
      candidates.push_back(cs);
    }
  }
  list<Rect> verified;
  for (auto it = candidates.begin(); it != candidates.end(); it++){
    Mat skewedRegion = text(it->r), correctedRegion;
    double angle = counterRotAngle(skewedRegion);
    rotate(skewedRegion, correctedRegion, angle);
    if (verifyRLT(correctedRegion, im, textData, *it)){
      verified.push_back(it->r);
    }
  }
  return verified;
}
