#ifndef TRACK_TEST_ENGINE
#define TRACK_TEST_ENGINE

#include <string>
#include <list>
#include <vector>
#include <map>
#include <functional>
#include <opencv2/opencv.hpp>

const std::string xml = "-str.xml";
const std::string pdf = ".pdf";
const std::string png = ".png";
const std::string tiff = ".tiff";

namespace testutils{
  struct Page{
    std::list<cv::Rect> tables;
    std::list<cv::Rect> gt;
  };
  
  struct Document{
    std::string name;
    std::map<int, Page>  pages;
    void insert(int pageNumber, Page page);
    void insertGT(int pageNumber, cv::Rect boundingBox);
  };
  
  std::set<std::string> files_in_folder(std::string path, std::function<bool(std::string&)> filter);
  bool isType(const std::string type, const std::string& name);
  std::string boundName(std::string str);
  std::string getBase(std::string str);
  std::string getName(std::string str);
  int getNumber(std::string str);
  void attachGT(Document& doc);
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
