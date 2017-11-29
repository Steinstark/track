#include <iostream>
#include <vector>
#include <sstream>
#include <opencv2/opencv.hpp>
#include "test_engine.hpp"
#include "detect_table.hpp"

using namespace std;
using namespace cv;

using vvr = vector<vector<Rect> >;

int main(int argc, char** argv){
  vector<string> files = files_in_folder(string(argv[1]));
  for (string file : files){
    vvr gt, cells;
    vector<string> text;
    getGroundTruth(file, gt, cells, text);
    save(file+pdf, file+png);
    for (int i = 0; i < gt.size(); i++){
      stringstream ss;
      ss << file << "_" << i << png;
      vector<Rect> d = detect_tables(ss.str());
      cout << compare(gt[i], d) << endl;
    }
  }
  return 0;
}
 
