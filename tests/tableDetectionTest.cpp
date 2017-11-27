#include <iostream>
#include "detect_table.hpp"


//TODO
//implement getGroundTruth
vector<Rect> getGroundTruth(string file){
  getData(file, collector, collection)
}

int main(int argc, char** argv){
  vector<string> files =files_in_folder(string(argv[1]));
  Stats global
  for (string file : files){
    vector<Rect> rgt = getGroundTruth(file)
    vector<Rect> m = detect_tables(file);
    Stats local = compare(m, rgt);
    global += local;    
  }
  cout << stats << endl;
  return 0;
}
