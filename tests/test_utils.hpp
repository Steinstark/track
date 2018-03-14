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
  bool isType(const std::string type, const std::string& name);
}

#endif
