#include <iostream>
#include <string>
#include <set>
#include <list>
#include <vector>
#include <sstream>
#include <functional>
#include <opencv2/opencv.hpp>
#include "test_utils.hpp"
#include "dataset_utils.hpp"
#include "detect_table.hpp"
#include "image_util.hpp"

using namespace std;
using namespace cv;
using namespace testutils;
using namespace dataset;

bool DISPLAY = false;
bool SAVE = false;

Page detectPage(const string& filename){
  Mat img = imread(filename.c_str());
  Mat bw = color2binary(img);
  Page page;
  page.tables = detect_tables(bw);
  page.size = img.size();
  return page;
}

Document detectDocument(set<string>::iterator first, set<string>::iterator last){
  Document doc(getBase(*first));
  while (first != last){
    Page p = detectPage(*first);
    int n = getNumber(*first);
    doc.insert(n, p);
    first++;
  }
  attachGT(doc);
  if (DISPLAY || SAVE){
    namedWindow("page", WINDOW_NORMAL);
    for (pair<int, Page>  p : doc.pages){
      string filename = doc.name + "_" + to_string(p.first) + ".png"; 
      Mat img = imread(filename);
      for (Rect r : p.second.tables){
	rectangle(img, r, Scalar(255, 0, 0), 5);      
      }
      for (Rect r : p.second.gt){
	rectangle(img, r, Scalar(0, 255, 0), 3);
      }
      if (DISPLAY){
	imshow("page", img);
	waitKey(0);
      }
      if (SAVE){
	imwrite(getDir(filename)+"/output/"+getFile(filename), img);
      }
    }
  }
  return doc;
}

//Maybe multithread
list<Document> detectAll(set<string>& files){
  auto first = files.begin();
  list<Document> documents;
  double precision = 0, recall = 0;
  cout << header() << endl;
  while (first != files.end()){
    auto last = files.upper_bound(boundName(*first));
    Document doc = detectDocument(first, last);
    Result result = doc.evaluate();
    cout << getBase(getName(*first)) << "\t" << result << endl;
    documents.push_back(doc);
    precision += (result.precision() - precision)/documents.size();
    recall += (result.recall() - recall)/documents.size();
    first = last;
  }
  cout << "Precision: " << precision << " Recall: " << recall << endl;
  return documents;
}

int main(int argc, char** argv){
  for (int i = 2; i < argc; i++){
    string flag(argv[i]);
    if (flag == "display") DISPLAY = true;
    else if (flag == "save") SAVE = true;
  }
  string path(argv[1]);
  string file = getFile(path);
  set<string> files;    
  if (isDir(path)){
    files = files_in_folder(path, bind(isType, "png", placeholders::_1));
  } else{
    files = files_in_folder(getDir(path), [&file](string& s){return shareBase(file, s) && isType("png", s);});
  }
  list<Document> documents = detectAll(files);
  return 0;
}
 
