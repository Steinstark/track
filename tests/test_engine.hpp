#ifndef TRACK_TEST_ENGINE
#define TRACK_TEST_ENGINE

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

const std::string xml = "-str.xml";
const std::string pdf = ".pdf";
const std::string png = ".png";
const std::string tiff = ".tiff";

std::vector<std::string> files_in_folder(std::string path);

cv::Rect translate(std::string pdf, cv::Rect bb);

double compare(const std::vector<cv::Rect >& gt, const std::vector<cv::Rect >& d);

std::vector<std::string> save(std::string in, std::string out);

void getGroundTruth(std::string file,
		    std::vector<std::vector<cv::Rect> >& table_regions,
		    std::vector<std::vector<cv::Rect> >& cells,
		    std::vector<std::string>& text);


#endif
