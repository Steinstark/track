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
      rectangle(tableCopy, tls[i].getBox()-cs.r.tl(), Scalar(255));
    }
    imshow("textlines", tableCopy);
    waitKey(0);
    return verticalArrangement(region, tls);  
  }
  return false;
}

vector<int> findRLT(Mat& text,
		    vector<ComponentStats>& textData,
		    vector<ComponentStats>& nonTextData,
		    ImageMeta& im)
{
  vector<int> candidates;
  for (int i = 0; i < nonTextData.size(); i++){
    if (candidateRLT(im, nonTextData[i])){
      candidates.push_back(i);
    }
  }
  vector<int> verified;
  for (int i = 0; i < candidates.size(); i++){
    Mat skewedRegion = text(nonTextData[candidates[i]].r), correctedRegion;
    double angle = counterRotAngle(skewedRegion);
    rotate(skewedRegion, correctedRegion, angle);
    if (verifyRLT(correctedRegion, im, textData, nonTextData[candidates[i]])){
      verified.push_back(candidates[i]);
    }
  }
  return verified;
}

void findNRLT(Mat& text, vector<ComponentStats>& data, ImageMeta& im){
  
}

vector<Rect> detect(Mat& text, Mat& nontext){
  vector<ComponentStats> textData = statistics(text);
  vector<ComponentStats> nontextData = statistics(nontext);
  ImageMeta meta(text.cols, text.rows, textData, nontextData);
  vector<int> rlt = findRLT(text, textData, nontextData, meta);
  vector<Rect> tables;
  for (int i = 0; i < rlt.size(); i++){
    tables.push_back(nontextData[rlt[i]].r);
  }
  findNRLT(text, nontextData, meta);
  return tables;
}
