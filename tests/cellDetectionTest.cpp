#include <iostream>
#include <vector>
#include <string>
#include "test_engine.hpp"
#include "detect_cell.hpp"

using namespace std;
using namespace cv;

using vvr = vector<vector<Rect> >;

int main(int argc, char** argv){
  vector<string> files = files_in_folder(string(argv[1]));
  for (string file : files){
    vvr tables, cells;
    vector<string> text;
    getGroundTruth(file, tables, cells, text);
    save(file+pdf, file+png);
    int index = 0;
    for (int i = 0; i < tables.size(); i++){
      for (int j = 0; j < tables[i].size(); j++){
	stringstream ss;
	ss << file << "_" << i << png;
	vector<Rect> d = detect_cells(ss.str(),tables[i][j]);
	cout << compare(cells[index], d) << endl;
	index++;
      }
    }
  }
  return 0;
}
