#include <string>
#include <list>
#include <opencv2/opencv.hpp>
#include "heuristic_filter.hpp"
#include "homogenous_regions.hpp"
#include "mla.hpp"
#include "mlc.hpp"
#include "image_util.hpp"

using namespace std;
using namespace cv;

const string outpath("/home/stein/Documents/th/Figures/");

void grayPlot(const Mat& img){
  imwrite(outpath + "gray.png", img);	  
}

void binaryPlot(const Mat& img){
  imwrite(outpath + "binary.png", img);
}

void heuristicPlot(const Mat& text, const Mat& nontext){
  imwrite(outpath + "heuristic_text.png", text);
  imwrite(outpath + "heuristic_nontext.png", nontext);
}

void homogenousPlot(const Mat& img, const list<Rect> regions){
  Mat local = img.clone();
  for (const Rect& r : regions){
    rectangle(local, r, Scalar(255));
  }
  imwrite(outpath + "homogenous.png", local);
}

void analysisPlot(const Mat& text, const Mat& nontext){
  imwrite(outpath + "analysis_text.png", text);
  imwrite(outpath + "analysis_nontext.png", nontext);
}

void classificationPlot(const Mat& text, const Mat& nontext){
  imwrite(outpath + "classification_text.png", text);
  imwrite(outpath + "classification_nontext.png", nontext);
}

int main(int argc, char** argv){
  string file(argv[1]);
  Mat img = imread(file.c_str());
  Mat gray;
  cvtColor(img, gray, COLOR_BGR2GRAY);
  grayPlot(gray);
  Mat text = gray2binary(gray);
  binaryPlot(text);
  Mat nontext(text.size(), CV_8UC1, Scalar(0));
  heuristic_filter(text, nontext);
  heuristicPlot(text, nontext);
  list<Rect> regions = homogenous_regions(text);
  homogenousPlot(text, regions);
  multi_level_analysis(text, nontext);
  analysisPlot(text, nontext);
  multi_level_classification(text, nontext);
  classificationPlot(text, nontext);
  return 0;
}
