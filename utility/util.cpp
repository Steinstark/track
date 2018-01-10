#include "util.hpp"

using namespace std;
using namespace cv;

bool fileHasType(string file, string extension){
  int pos = file.find_last_of(".");
  return file.substr(pos+1) == extension;
}

Rect pos2rect(int l, int t, int r, int b){
  return Rect(l, t, r-l, b-t);
}
