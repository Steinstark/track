#include "test_engine.hpp"
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include "RTree.h"
#include "pugixml.hpp"
#include <Magick++.h>

using namespace std;
using namespace cv;
using namespace boost::filesystem;
using namespace Magick;
using namespace pugi;

using vvr = vector<vector<Rect> >;

string remove_extension(const string& file){
  size_t lastdot = file.find_last_of(".");
  if (lastdot == string::npos) return file;
  return file.substr(0, lastdot);
}

bool isPDF(const string& file){
  size_t lastdot = file.find_last_of(".");
  return file.substr(lastdot) == ".pdf";
}

vector<string> files_in_folder(string dir){
  set<string> unique_files;
  path p(dir);
  directory_iterator end_itr;
  for (directory_iterator itr(p); itr != end_itr; ++itr){
    string file = canonical(itr->path()).string();      
    if (is_regular_file(file) && isPDF(file)){
      unique_files.insert(remove_extension(file));
    }
  }
  return vector<string>(unique_files.begin(), unique_files.end());
}

bool callback(int id, void* arg){
  vector<int>* v = static_cast<vector<int>*>( arg );
  v->push_back(id);
  return true;
}

bool isInside(const Rect& outer, const Rect& inner){
  return outer.x <= inner.x &&
    outer.y <= inner.y &&
    outer.br().x >= inner.br().x &&
    outer.br().y >= inner.br().y;
}

double compare(const vector<Rect>& gt, const vector<Rect>& d){
  RTree<int, int, 2, float> tree;
  int incorrect = 0;
  int correct = 0;
  for (int i = 0; i < gt.size(); i++){
    Rect r = gt[i];
    int tl[] = {r.x, r.y};
    int br[] = {r.br().x, r.br().y};
    tree.Insert(tl,br,i);    
  }
  for (Rect r : d){
    int tl[] = {r.x, r.y};
    int bl[] = {r.br().x, r.br().y};
    vector<int> vec;
    int hits = tree.Search(tl, bl, callback, (static_cast<void*>(&vec)));
    if (hits == 1 && isInside(r, gt[vec[0]])){
      correct++;
    }else
      incorrect++;
  }
  if (correct + incorrect == 0)
    return 1;
  return (double)correct/(correct + incorrect);
}

Rect translate(int height, Rect bb){
  return Rect(bb.x, height - bb.y, bb.width, bb.height); 
}

vector<string> save(string in, string out){
  InitializeMagick("--quiet");
  vector<Image> imageList;
  readImages(&imageList, in);
  int index = out.find_last_of(".");
  vector<string> files;
  for (int i = 0; i < imageList.size(); i++){    
    ostringstream oss;
    oss << out.substr(0, index) << "_" << i << out.substr(index);    
    imageList[i].write(oss.str());
    files.push_back(oss.str());
  }  
  return files;
}

int getHeight(string pdf){
  InitializeMagick(NULL);
  Image img(pdf);
  return img.rows();  
}

void getGroundTruth(string file, vvr& table_regions, vvr& cells, vector<string>& text){
  xml_document doc;
  const int inf = 1000000;
  doc.load_file((file+xml).c_str());
  xpath_node_set rns = doc.select_nodes("//region");
  vector<Rect> page;
  int page_index = 1;
  int height = getHeight(file+pdf);
  for (xpath_node rn : rns){
    while (rn.node().attribute("page").as_int() > page_index){
      table_regions.push_back(page);
      page.clear();
      page_index++;
    }
    int left = inf, right = -inf , top = -inf, bottom = inf;
    xpath_node_set bns = rn.node().select_nodes("cell/bounding-box");
    vector<Rect> table;
    for (xpath_node bn : bns){
      int l = bn.node().attribute("x1").as_int();
      int r = bn.node().attribute("x2").as_int();
      int b = bn.node().attribute("y1").as_int();      
      int t = bn.node().attribute("y2").as_int();
      text.push_back(bn.node().parent().child_value("content"));
      table.push_back(translate(height, Rect(l, t, r-l,t-b)));
      left = min(left, l);
      right = max(right, r);
      bottom = min(bottom, b);
      top = max(top, t);
    }
    cells.push_back(table);
    page.push_back(translate(height, Rect(left, top, right-left, top-bottom)));
  }
  table_regions.push_back(page);
}
