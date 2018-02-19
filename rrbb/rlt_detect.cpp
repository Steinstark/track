#include "rlt_detect.hpp"
#include <list>
#include <vector>
#include <functional>
#include <opencv2/opencv.hpp>
#include "utility.hpp"
#include "image_primitives.hpp"
#include "image_util.hpp"
#include "text_tools.hpp"
#include "tree_helper.hpp"

using namespace std;
using namespace cv;
using namespace tree;

bool candidateRLT(ImageMeta& im, ComponentStats& cs){
  return
    containsManyElements(im.t_tree, cs) &&
    (isHorizontalLine(cs) ||
     regionIsRectangle(cs));
}

list<Rect> findRLT(ImageDataBox& imd, ImageMeta& im, function<bool(Mat&, ComponentStats&)> f)
{
  list<ComponentStats> candidates;
  vector<ComponentStats>& nontextData = imd.nontextData;
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
    if (verifyReg(correctedRegion)){
      verified.push_back(it->r);
    }
  }
  return verified;
}
