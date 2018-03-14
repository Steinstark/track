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
