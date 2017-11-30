#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>
#include "textbox_ocr.hpp"

using namespace std;

vector<string> textbox_content(string path, const vector<cv::Rect>& rv){
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  if (api->Init(NULL, "eng")) {
    fprintf(stderr, "Could not initialize tesseract.\n");
    exit(1);
  }  
  Pix *image = pixRead(path.c_str());
  api->SetImage(image);
  vector<string> v;
  //  cv::Mat img = cv::imread(path.c_str());

  for (cv::Rect r : rv){
    //cv::Scalar color = cv::Scalar(255,0,0);
    //cv::rectangle(img, r.tl(), r.br(), color, 2,8,0);
    //cv::imshow("img", img);
    //cv::waitKey(0);
    api->SetRectangle(r.x, r.y, r.width, r.height);
    string s(api->GetUTF8Text());
    s.erase(remove(s.begin(), s.end(), '\n'), s.end());
    v.push_back(s);    
  }
  api->End();
  pixDestroy(&image);  
  return v;
}
