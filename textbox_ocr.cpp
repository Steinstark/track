#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>
#include "textbox_ocr.hpp"

using namespace std;

vector<string> textbox_content(string path, cv::Rect table, vector<cv::Rect> rv){
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  if (api->Init(NULL, "eng")) {
    fprintf(stderr, "Could not initialize tesseract.\n");
    exit(1);
  }  
  Pix *image = pixRead(path.c_str());
  api->SetImage(image);
  cv::Point pt = table.tl();
  vector<string> v;
  for (cv::Rect r : rv){
    cv::Point pc = r.tl();
    api->SetRectangle(pt.x+pc.x, pt.y+pc.y, r.width, r.height);
    v.push_back(string(api->GetUTF8Text()));    
  }
  api->End();
  pixDestroy(&image);  
  return v;
}
