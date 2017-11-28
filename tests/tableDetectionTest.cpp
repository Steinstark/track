#include <iostream>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "test_engine.hpp"
#include "pugixml.hpp"
#include "detect_table.hpp"

using namespace std;
using namespace cv;
using namespace pugi;

const int inf = 1000000;
const string xml = "-str.xml";
const string pdf = ".pdf";
const string png = ".png";

//TODO
//implement getGroundTruth
vector<Rect> getGroundTruth(string file){
  xml_document doc;
  doc.load_file((file+xml).c_str());
  xpath_node_set rns = doc.select_nodes("//region");
  vector<Rect> tables;
  for (xpath_node rn : rns){
    int left = inf, right = -inf , top = -inf, bottom = inf;
    xpath_node_set bns = rn.node().select_nodes("cell/bounding-box");
    for (xpath_node bn : bns){
      int l = bn.node().attribute("x1").as_int();
      int r = bn.node().attribute("x2").as_int();
      int b = bn.node().attribute("y1").as_int();      
      int t = bn.node().attribute("y2").as_int();
      left = min(left, l);
      right = max(right, r);
      bottom = min(bottom, b);
      top = max(top, t);
    }
    tables.push_back(translate(file+pdf, Rect(left, top, right-left, top-bottom)));
  }
  return tables;
}

//TODO
int compare(const vector<Rect>& gt, const vector<Rect>& d){
  sort(d.begin(), d.end(), 
}


int main(int argc, char** argv){
  vector<string> files = files_in_folder(string(argv[1]));
  vector<Rect> d;
  for (string file : files){
    vector<Rect> gt = getGroundTruth(file);
    save(file+pdf, file+png);
    vector<Rect> d = detect_tables(file+png);
    compare(gt, d);
  }
  //  cout << stats << endl;
  return 0;
}

