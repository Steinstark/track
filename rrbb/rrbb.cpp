#include "detect_table.hpp"
#include <list>
#include "heuristic_filter.hpp"
#include "mla.hpp"
#include "mlc.hpp"
#include "detect.hpp"

using namespace std;
using namespace cv;

list<Rect> detect_tables(Mat& text){
  Mat nontext(text.size(), CV_8UC1, Scalar(0));
  heuristic_filter(text, nontext);
  multi_level_analysis(text, nontext);
  multi_level_classification(text, nontext);
  return detect(text, nontext);
}
