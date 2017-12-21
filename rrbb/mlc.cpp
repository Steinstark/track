#include <vector>
#include <opencv2/opencv.hpp>

#include "mlc.hpp"
#include "homogenous_regions.hpp"
#include "recursive_filter.hpp"

using namespace std;
using namespace cv;

void multi_level_classification(Mat& text, Mat& nontext){
  const double TOL = 0.99;
  while(1){
    vector<Rect> regions = homogenous_regions(text);
    int total = 0, same = 0;
    for (Rect r : regions){
      Mat ltext = text(r), lnontext = nontext(r);
      same += recursive_filter(ltext, lnontext);
      total++;
    }
    if ((double)total/same >= TOL)
      break;
  }
}
