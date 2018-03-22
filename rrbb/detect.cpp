#include "detect.hpp"
#include <list>
#include <vector>
#include <functional>
#include <opencv2/opencv.hpp>
#include "utility.hpp"
#include "image_primitives.hpp"
#include "image_util.hpp"
#include "text_tools.hpp"
#include "tree_helper.hpp"
#include "rlt_detect.hpp"
#include "clt_detect.hpp"
#include "nrlt_detect.hpp"

using namespace std;
using namespace cv;
using namespace tree;

list<Rect> detect(Mat& text, Mat& nontext){
  ImageDataBox imd(text, nontext);
  ImageMeta im(text.cols, text.rows, imd.textData, imd.nontextData);
  list<Rect> nrlTables = findNRLT(text, nontext, im);
  return nrlTables;
}
