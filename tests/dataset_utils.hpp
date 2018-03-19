#ifndef TRACK_DATASET_UTILS
#define TRACK_DATASET_UTILS

#include <string>
#include <list>
#include <map>
#include <opencv2/opencv.hpp>

namespace dataset{
  
  struct Result{
    int countGT, correct, incorrect, pure, complete;
    Result(int countGT): countGT(countGT), correct(0), incorrect(0), pure(0), complete(0) {}
    Result& operator+=(const Result& other);
    double recall();
    double precision();
    void update(bool pure, bool complete);
    friend std::ostream& operator<< (std::ostream& stream, const Result& result){
      stream << result.correct << "\t"
	     << result.incorrect << "\t"
	     << result.pure << "\t"
	     << result.complete << "\t"
      	     << result.countGT;
      return stream;
    }
  };

  std::string header();
  
  struct Page{
    cv::Size size;
    std::list<cv::Rect> tables;
    std::list<cv::Rect> gt;
    Result evaluate();
  };
  
  struct Document{
    std::string name;
    std::map<int, Page>  pages;
    Document(std::string name): name(name){}
    void insert(int pageNumber, Page page);
    void insertGT(int pageNumber, cv::Rect boundingBox);
    void attachGT();
    Result evaluate();
    
  };

  std::string boundName(std::string str);
  std::string getBase(std::string str);
  std::string getName(std::string str);
  int getNumber(std::string str);
  void attachGT(Document& doc);
}

#endif
