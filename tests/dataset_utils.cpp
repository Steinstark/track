#include "dataset_utils.hpp"
#include <string>
#include <sstream>
#include <utility>
#include <cmath>
#include <opencv2/opencv.hpp>
#include "image_util.hpp"
#include "pugixml.hpp"
#include "tree_helper.hpp"

using namespace std;
using namespace dataset;
using namespace pugi;
using namespace tree;
using namespace cv;

Result& dataset::Result::operator+=(const Result& other){
  countGT += other.countGT;
  correct += other.correct;
  incorrect += other.incorrect;
  pure += other.pure;
  complete += other.complete;  
  return *this;
}

void dataset::Result::update(bool isPure, bool isComplete){
  if (isPure)
    pure++;
  if (isComplete)
    complete++;
  if (isPure && isComplete)
    correct++;
  else
    incorrect++;
}


string dataset::header(){
  return "Name\tCorrect\tIncorrect\tPure\tComplete\tCount";
}

Result dataset::Page::evaluate(){
  Result result(gt.size());
  RTBox tree;
  for (Rect& table : gt){
    insert2tree(tree, table);    
  }
  for (Rect& table : tables){
    bool pure, complete = false;
    list<Rect> hits = search_tree(tree, table);
    pure = hits.size() == 1;
    for (Rect hit : hits){
      if  (hit.area()*0.95 <= (hit & table).area()){
	complete = true;
	break;
      }
    }
    result.update(pure, complete);
  }
  return result;
}

double dataset::Result::precision(){
  if (correct + incorrect == 0){
    return 1;
  }
  return correct/(correct + incorrect);
}

double dataset::Result::recall(){
  if (countGT == 0){
    return 1;
  }
  return correct/countGT;
}

void dataset::Document::insert(int pageNumber, Page p){
  pages[pageNumber] = p;
}

void dataset::Document::insertGT(int pageNumber, Rect table){
  pages[pageNumber].gt.push_back(table);
}

Result dataset::Document::evaluate(){
  Result result(0);
  for (pair<int, Page> p : pages){
    result += p.second.evaluate();
  }
  return result;
}

string dataset::getBase(string str){
  return str.substr(0, str.find_last_of("_"));
}

string dataset::getName(string str){
  return str.substr(str.find_last_of("/")+1);
}

int dataset::getNumber(string str){
  int first = str.find_last_of("_")+1;
  int last = str.find_last_of(".");
  int numb;
  istringstream(str.substr(first, last-first)) >> numb;
  return numb;
}

string dataset::boundName(string str){
  return getBase(str) + "_" + "z"; 
}

Rect scaleAndTranslate(Rect& r, Size size){
  double sf = (double)150/72;
  int unscaledBottom = round(size.height/sf);
  int ty = (unscaledBottom - r.br().y);
  return Rect(r.x*sf, ty*sf, r.width*sf, r.height*sf);
}

void dataset::attachGT(Document& doc){
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
    Rect fixedBox =  scaleAndTranslate(r, doc.pages[number].size);
    doc.insertGT(rn.node().attribute("page").as_int(), fixedBox);
  }
}
