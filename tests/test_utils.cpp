#include "test_utils.hpp"
#include <string>
#include <set>
#include <boost/filesystem.hpp>


using namespace std;
using namespace boost::filesystem;


string remove_extension(const string& file){
  size_t lastdot = file.find_last_of(".");
  if (lastdot == string::npos) return file;
  return file.substr(0, lastdot);
}

bool testutils::isType(const string type, const string& name){
  size_t lastdot = name.find_last_of(".");
  return name.substr(lastdot+1) == type;
}

bool testutils::shareBase(const string& base, const string& other){
  bool shared = other.find(base) != string::npos;
  return shared;
}

bool testutils::isDir(const string& path){
  return path.find_last_of("/") == path.size()-1;
}

string testutils::getDir(const string& path){
  int pos = path.find_last_of("/");
  if (pos == string::npos)
    return "";
  return path.substr(0, pos+1);
}

string testutils::getFile(const string& path){
  int pos = path.find_last_of("/");
  if (pos == string::npos)
    return path;
  if (pos == path.size() -1)
    return "";
  return path.substr(pos+1);
}

set<string> testutils::files_in_folder(string dir, function<bool(string&)> filter = [](string&){return true;}){
  set<string> unique_files;
  path p(dir);
  directory_iterator end_itr;
  for (directory_iterator itr(p); itr != end_itr; ++itr){
    string file = canonical(itr->path()).string();      
    if (is_regular_file(file) && filter(file)){
      unique_files.insert(file);
    }
  }
  return unique_files;
}
