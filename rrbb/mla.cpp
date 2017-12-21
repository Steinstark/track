#include <vector>
#include <opencv2/opencv.hpp>

#include "mla.hpp"
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
    Mat region = text(r);
    vector<Rect> homboxes = homogenous_regions(region);
    for (int i = 0; i < homboxes.size(); i++){
      Rect lr = homboxes[i]+r.tl();
      Mat ltext = text(lr), lntext = ntext(lr);
      if (recursive_filter(ltext, lntext))
	q.push(lr);
    }
  }
}
