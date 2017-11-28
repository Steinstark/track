#include "test_engine.hpp"
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include <Magick++.h>


using namespace std;
using namespace cv;
using namespace boost::filesystem;
using namespace Magick;

//TODO
//implement files_in_folder
//implement compare

string remove_extension(const string& file){
  size_t lastdot = file.find_last_of(".");
  if (lastdot == string::npos) return file;
  return file.substr(0, lastdot);
}

bool isPDF(const string& file){
  size_t lastdot = file.find_last_of(".");
  return file.substr(lastdot) == ".pdf";
}

vector<string> files_in_folder(string dir){
  set<string> unique_files;
  path p(dir);
  directory_iterator end_itr;
  for (directory_iterator itr(p); itr != end_itr; ++itr){
    string file = canonical(itr->path()).string();      
    if (is_regular_file(file) && isPDF(file)){
      unique_files.insert(remove_extension(file));
    }
  }
  return vector<string>(unique_files.begin(), unique_files.end());
}

Rect translate(string pdf, Rect bb){
  InitializeMagick(NULL);  
  Image img(pdf);
  int height = img.rows();
  return Rect(bb.x, height - bb.y, bb.width, bb.height); 
}

void save(string in, string out){
  InitializeMagick(NULL);  
  Image img(in);
  img.write(out);
}
