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

Page detectPage(const string pageName, int pageNumber){
  Mat img = imread(pageName.c_str());
  Mat bw = color2binary(img);
  Page page;
  page.tables = detect_tables(bw);
  page.number = pageNumber;
  return page;
}

Document detectDocument(set<string>::iterator first, set<string>::iterator last){
  Document doc;
  doc.name = getBase(*first);
  int number = 1;
  while (first != last){
    Page p = detectPage(*first, number);
    doc.pages.push_back(p);
    first++;
  }
  return doc;
}

//Maybe multithread
list<Document> detectAll(set<string>& files){
  auto first = files.begin();
  list<Document> documents;
  while (first != files.end()){
    auto last = files.upper_bound(boundName(*first));
    Document doc = detectDocument(first, last);
    cout << "Finished processing: " << getBase(*first) << endl;
    documents.push_back(doc);
    first = last;
  }
  return documents;
}

int main(int argc, char** argv){
  set<string> files = files_in_folder(string(argv[1]), bind(isType, "png", placeholders::_1));
  list<Document> documents = detectAll(files);
				      /*  for (string file : files){
    vvr gt, cells;
    vector<string> text;
    getGroundTruth(file, gt, cells, text);
    for (int i = 0; i < gt.size(); i++){
      stringstream ss;
      ss << file << "_" << i << tiff;
      vector<Rect> d = detect_tables(ss.str());
      cout << compare(gt[i], d) << endl;
    }
    }*/
  return 0;
}
 
