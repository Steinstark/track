#include <vector>
#include <algorithm>
#include <queue>

#include "detect_table.hpp"
#include "RTree.h"
#include "heuristic_filter.hpp"
#include "recursive_filter.hpp"
#include "mla.hpp"
#include "mlc.hpp"
#include "noise.hpp"
#include "detect.hpp"
#include "utility.hpp"

using namespace std;
using namespace cv;

vector<Rect> detect_tables(Mat& text){
  Mat nontext(text.size(), CV_8UC1, Scalar(0));
  heuristic_filter(text, nontext);
  //multi_level_analysis(text, nontext);
  //multi_level_classification(text, nontext);
  remove_noise(text, nontext);  
  return detect(text, nontext);
}
