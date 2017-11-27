#include <iostream>
#include <vector>
#include <string>
#include "test_engine.hpp"
#include "detect_cell.hpp"


using namespace std;
using namespace cv;
//TODO
//implement GT
//implement getGroundTrut1

int main(int argc, char** argv){
  vector<string> files = files_in_folder(string(argv[1]));
  Stats global;
  for (string file : files){
    GT gt = getGroundTruth(file);
    for (auto p : gt){      
      vector<Rect> m = detect_cells(file, p.first);
      Stats local = compare(m, p.second);
      global += local;
    }
  }
  cout << global << endl;
  return 0;
}
