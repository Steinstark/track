#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "test_engine.hpp"
#include "textbox_ocr.hpp"

using namespace std;
using namespace cv;

using vvr = vector<vector<Rect> >;

double textCompare(const vector<string>& d, const vector<string>& gt, int a, int b){
  int count = 0;
  for (int i = 0; i < d.size(); i++){
    if (d[i] == gt[i])
      count++;
  }
  return (double)count/gt.size();
}

int main(int argc, char** argv){
  vector<string> files = files_in_folder(string(argv[1]));
  for (string file : files){
    vvr tables, cells;
    vector<string> text;
    getGroundTruth(file, tables, cells, text);
    save(file+pdf, file+png);
    int index = 0, l = 0, r = 0;
    for (int i = 0; i < tables.size(); i++){
      vector<Rect> cellsOnPage;
      stringstream ss;
      ss << file << "_" << i << png;
      for (int j = 0; j < tables[i].size(); j++){
	r+= cells[index].size();
	cellsOnPage.insert(end(cellsOnPage), begin(cells[index]), end(cells[index]));
	index++;
      }
      vector<string> content = textbox_content(ss.str(), cellsOnPage);
      cout << textCompare(content, text, l, r-1) << endl;
      l = r;
    }
      
  }
  return 0;
}
