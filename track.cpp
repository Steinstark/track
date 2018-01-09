#include <string>
#include <opencv2/opencv.hpp>

#include "detect_table.hpp"
#include "detect_cell.hpp"
#include "textbox_ocr.hpp"
#include "find_grid.hpp"
#include "util.hpp"

#include <Magick++.h> 

using namespace std;
using namespace cv;
using namespace Magick;

Mat pdf2mat(string pdf){
  InitializeMagick(NULL);  
  Image image(pdf);
  int w = image.columns();
  int h = image.rows();
  Mat opencvImage(h,w,CV_8UC3);
  image.write(0 ,0 ,w ,h, "BGR", Magick::CharPixel, opencvImage.data);
  return opencvImage;
}

Mat gray2binary(const Mat& gray){
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 15, -2);
  return bw;
}

int main(int argc, char** argv){
  if (argc != 2){
    cout << "Invalid number of arguments" << endl;
    return 0;
  }
  string file(argv[1]);
  Mat img;
  if (fileHasType(file, "pdf"))
    img = pdf2mat(file);
  else
    img = imread(file.c_str());
  Mat gray;
  cvtColor(img, gray, COLOR_BGR2GRAY);
  if (!gray.data)
    cerr << "Problem loading image. " << endl;
  Mat bw = gray2binary(gray);
  vector<Rect> tables = detect_tables(bw);
  for (Rect& table : tables){
    vector<Rect> cells = detect_cells(bw, table);
    //should be cells instead of tables
    vector<string> content = textbox_content(bw, tables);   
    for (string s : content){
      cout << s << endl;
    }
    //find_grid(cells, content);
  }
  return 0;
}
