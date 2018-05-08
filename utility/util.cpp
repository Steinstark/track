#include "util.hpp"
#include <string>

using namespace std;

bool fileHasType(string file, string extension){
  int pos = file.find_last_of(".");
  return file.substr(pos+1) == extension;
}
