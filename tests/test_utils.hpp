#ifndef TRACK_TEST_ENGINE
#define TRACK_TEST_ENGINE

#include <string>
#include <list>
#include <vector>
#include <set>
#include <functional>
#include <opencv2/opencv.hpp>

const std::string xml = "-str.xml";
const std::string pdf = ".pdf";
const std::string png = ".png";
const std::string tiff = ".tiff";

namespace testutils{
  struct Page{
    int number;
    std::list<cv::Rect> tables;
  };
  
  struct Document{
    std::string name;
    std::list<Page>  pages;
  };
  
  std::set<std::string> files_in_folder(std::string path, std::function<bool(std::string&)> filter);
  bool isType(const std::string type, const std::string& name);
  std::string boundName(std::string str);
  std::string(getBase(std::string str));
}
//cv::Rect translate(std::string pdf, cv::Rect bb);

//double compare(const std::vector<cv::Rect >& gt, const std::vector<cv::Rect >& d);

//std::vector<std::string> save(std::string in, std::string out);

/*void getGroundTruth(std::string file,
		    std::vector<std::vector<cv::Rect> >& table_regions,
		    std::vector<std::vector<cv::Rect> >& cells,
		    std::vector<std::string>& text);

*/
#endif
