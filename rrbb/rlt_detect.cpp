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
    (isLine(cs) ||
     regionIsRectangle(cs));
}

list<Rect> findRLT(ImageDataBox& imd, ImageMeta& im)
{
  list<ComponentStats> candidates;
  vector<ComponentStats>& nontextData = imd.nontextData;
  Mat& text = imd.text, nontext = imd.nontext;
  for (ComponentStats& cs: nontextData){
    if (candidateRLT(im, cs)){
      candidates.push_back(cs);
    }
  }
  list<Rect> verified;
  for (auto it = candidates.begin(); it != candidates.end(); it++){
    Mat skewedText = text(it->r), skewedNontext = nontext(it->r), correctedText, correctedNontext;
    if (verifyReg(skewedText, skewedNontext, search_tree(im.nt_tree, it->r).size())){
      verified.push_back(it->r);
    }
  }
  return verified;
}
