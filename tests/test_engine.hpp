#ifndef TRACK_TEST_ENGINE
#define TRACK_TEST_ENGINE

#include <string>
#include <opencv2/opencv.hpp>

//TODO
//implement files_in_folder
//implement Stats
//implement compare
std::vector<std::string> files_in_folder(std::string path);
//Stats compare(cv::Rect a, cv::Rect b);
cv::Rect translate(std::string pdf, cv::Rect bb);
void save(std::string in, std::string out);


#endif
