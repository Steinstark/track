#include "detect.hpp"
#include <list>
#include <opencv2/opencv.hpp>
#include "utility.hpp"
#include "image_primitives.hpp"
#include "image_util.hpp"
#include "text_tools.hpp"
#include "rlt_detect.hpp"
#include "nrlt_detect.hpp"

using namespace std;
using namespace cv;
using namespace tree;

list<Rect> detect(Mat& text, Mat& nontext){
  ImageDataBox imd(text, nontext);
  ImageMeta im(text.cols, text.rows, imd.textData, imd.nontextData);
  list<Rect> tables = findRLT(text, nontext);
  Mat textClone = text.clone();
  for (Rect& r : tables){
    textClone(r) = 0;
    nontext(r) = 0;
  }
  // tables.splice(tables.end(),findNRLT(textClone, nontext, im));
  return tables;
}
