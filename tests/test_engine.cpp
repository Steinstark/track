#include <string>
#include <iostream>
#include <filesystem>

using namespace std;
using namespace cv;

//TODO
//implement files_in_folder
//implement compare

vector<string> files_in_folder(string path){
  vector<string> files;
  for (auto& p : directory_iterator(path)){
    files.push_back(p);
  }
  return files;
}

Stats compare(Rect a, Rect b){
  
}

int translate(int coord){
  
}
