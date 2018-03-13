#include "test_utils.hpp"
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include "image_util.hpp"
#include "pugixml.hpp"

using namespace std;
using namespace cv;
using namespace boost::filesystem;
using namespace pugi;

void testutils::Document::insert(int pageNumber, Page p){
  pages[pageNumber] = p;
}

void testutils::Document::insertGT(int pageNumber, Rect boundingBox){
  pages[pageNumber].gt.push_back(boundingBox);
}

string remove_extension(const string& file){
  size_t lastdot = file.find_last_of(".");
  if (lastdot == string::npos) return file;
  return file.substr(0, lastdot);
}

bool testutils::isType(const string type, const string& name){
  size_t lastdot = name.find_last_of(".");
  return name.substr(lastdot+1) == type;
}

set<string> testutils::files_in_folder(string dir, function<bool(string&)> filter = [](string&){return true;}){
  set<string> unique_files;
  path p(dir);
  directory_iterator end_itr;
  for (directory_iterator itr(p); itr != end_itr; ++itr){
    string file = canonical(itr->path()).string();      
    if (is_regular_file(file) && filter(file)){
      unique_files.insert(file);
    }
  }
  return unique_files;
}

string testutils::getBase(string str){
  return str.substr(0, str.find_last_of("_"));
}

string testutils::getName(string str){
  return str.substr(str.find_last_of("/")+1);
}

int testutils::getNumber(string str){
  int first = str.find_last_of("_");
  int last = str.find_last_of(".");
  int numb;
  istringstream(str.substr(first, last-first)) >> numb;
  return numb;
}

string testutils::boundName(string str){
  return getBase(str) + "_" + "z"; 
}

Rect scaleAndTranslate(Rect& r){
  return r;
}

void testutils::attachGT(Document& doc){
  const string reg = "-reg.xml";
  const string text = "-str.xml";
  const string regname = doc.name + reg;
  const string textname = doc.name + text;
  xml_document xml;
  xml.load_file(regname.c_str());
  xpath_node_set rns = xml.select_nodes("//region");
  for (xpath_node rn : rns){
    xml_node region = rn.node();
    int number =  region.attribute("page").as_int();
    xml_node b = region.child("bounding-box");
    Rect r = pos2rect(b.attribute("x1").as_int(),
		      b.attribute("y1").as_int(),
		      b.attribute("x2").as_int(),
		      b.attribute("y2").as_int());
    Rect fixedBox =  scaleAndTranslate(r);
    doc.insertGT(rn.node().attribute("page").as_int(), fixedBox);
  }
}

/*
bool isInside(const Rect& outer, const Rect& inner){
  return outer.x <= inner.x &&
    outer.y <= inner.y &&
    outer.br().x >= inner.br().x &&
    outer.br().y >= inner.br().y;
}

double compare(const vector<Rect>& gt, const vector<Rect>& d){
  /* RTree<int, int, 2, float> tree;
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
    return (double)correct/(correct + incorrect);*
  return 0.0;
}

Rect translate(int height, Rect bb){
  return Rect(bb.x, height - bb.y, bb.width, bb.height); 
}

void getGroundTruth(string file, vvr& table_regions, vvr& cells, vector<string>& text){
  xml_document doc;
  const int inf = 1000000;
  doc.load_file((file+xml).c_str());
  xpath_node_set rns = doc.select_nodes("//region");
  vector<Rect> page;
  int page_index = 1;
  int height = 1;//getHeight(file+pdf);
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
*/
