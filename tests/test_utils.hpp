#ifndef TRACK_TEST_ENGINE
#define TRACK_TEST_ENGINE

#include <string>
#include <set>
#include <functional>

const std::string xml = "-str.xml";
const std::string pdf = ".pdf";
const std::string png = ".png";
const std::string tiff = ".tiff";

namespace testutils{
  std::set<std::string> files_in_folder(std::string path, std::function<bool(std::string&)> filter);
  std::string getDir(const std::string& path);
  std::string getFile(const std::string& path);
  bool isType(const std::string type, const std::string& name);
  bool isDir(const std::string& path);
  bool shareBase(const std::string& base, const std::string& other);
}

#endif
