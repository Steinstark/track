#include "rlt_detect.hpp"
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
