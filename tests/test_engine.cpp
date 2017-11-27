#include <string>
#include <vector>
#include <iostream>
#include <boost/filesystem.hpp>

using namespace std;
//using namespace cv;
using namespace boost::filesystem;

//TODO
//implement files_in_folder
//implement compare

vector<string> files_in_folder(string dir){
  vector<string> files;
  path p(dir);
  directory_iterator end_itr;
  for (directory_iterator itr(p); itr != end_itr; ++itr){
    if (is_regular_file(itr->path())){
      files.push_back(itr->path().string());
    }
  }
  return files;
}

  /*Stats compare(Rect a, Rect b){
  
    }

int translate(int coord){
  
}*/
