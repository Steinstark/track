#include <iostream>
#include <vector>
#include <string>
#include "test_engine.hpp"
#include "textbox_ocr.hpp"

using namespace std;
using namespace cv;

//TODO
//implement getGroundTruth

using vector<pair<Rect,string> > = Zip;

vector<string> getGroundTruth(string file){
  ifstream ifs(file);
  string xml;
  ifs >> xml;
  //for each textbox
  //get goordinate and content
  //translate the coordinate
  //put coordinates in a map linking to content
  //sorted based on x first and y second

}

int main(int argc, char** argv){
  vector<string> files = files_in_folder(string(argv[1]));
  int c, tc;
  for (string file : files){
    Zip gt = getGroundTruth(file);
    vector<string> tgt;
    vector<Rect> rgt;
    for (auto p : gt){
      tgt.push_back(p.first);
      rgt.push_back(p.second);
    }
    vector<string> m = textbox_content(path, rgt);
    int correct = 0;
    for (int i = 0; i < tgt.size(); i++){
      if (tgt[i] == m[i])
	count++;
    }
    tc += tgt.size();
  }
  double recall = c/tc
  cout << "OCR recall: " << recall << endl;
}
