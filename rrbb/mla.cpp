#include "mla.hpp"

#include <list>
#include <opencv2/opencv.hpp>
#include "homogenous_regions.hpp"
#include "recursive_filter.hpp"
#include "utility.hpp"

using namespace std;
using namespace cv;

void multi_level_analysis(Mat& text, Mat& ntext){
  queue<Rect> q;
  q.push(Rect(0, 0, text.cols, text.rows));
  while (!q.empty()){
    Rect r = q.front();
    q.pop();
    Mat imageRegion = text(r);
    list<Rect> regions = homogenous_regions(imageRegion);
    for (Rect region : regions){
      Rect lr = region+r.tl();
      Mat ltext = text(lr), lntext = ntext(lr);
      if (recursive_filter(ltext, lntext))
	q.push(lr);
    }
  }
}
