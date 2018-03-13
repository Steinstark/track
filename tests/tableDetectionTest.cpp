#include <iostream>
#include <string>
#include <set>
#include <list>
#include <vector>
#include <sstream>
#include <functional>
#include <opencv2/opencv.hpp>
#include "test_utils.hpp"
#include "detect_table.hpp"
#include "image_util.hpp"

using namespace std;
using namespace cv;
using namespace testutils;

Page detectPage(const string filename){
  Mat img = imread(filename.c_str());
  Mat bw = color2binary(img);
  Page page;
  page.tables = detect_tables(bw);
  return page;
}

Document detectDocument(set<string>::iterator first, set<string>::iterator last){
  Document doc;
  doc.name = getBase(*first);
  while (first != last){
    Page p = detectPage(*first);
    int n = getNumber(*first);
    doc.insert(n, p);
    first++;
  }
  attachGT(doc);
  return doc;
}

//Maybe multithread
list<Document> detectAll(set<string>& files){
  auto first = files.begin();
  list<Document> documents;
  while (first != files.end()){
    auto last = files.upper_bound(boundName(*first));
    Document doc = detectDocument(first, last);
    cout << "Finished processing: " << getBase(getName(*first)) << endl;
    documents.push_back(doc);
    first = last;
  }
  return documents;
}

int main(int argc, char** argv){
  set<string> files = files_in_folder(string(argv[1]), bind(isType, "png", placeholders::_1));
  list<Document> documents = detectAll(files);
  return 0;
}
 
