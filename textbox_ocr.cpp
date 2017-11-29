#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>
#include "textbox_ocr.hpp"

using namespace std;

vector<string> textbox_content(string path, vector<cv::Rect> rv){
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  if (api->Init(NULL, "eng")) {
    fprintf(stderr, "Could not initialize tesseract.\n");
    exit(1);
  }  
  Pix *image = pixRead(path.c_str());
  api->SetImage(image);
  vector<string> v;
  for (cv::Rect r : rv){
    api->SetRectangle(r.x, r.y, r.width, r.height);
    v.push_back(string(api->GetUTF8Text()));    
  }
  api->End();
  pixDestroy(&image);  
  return v;
}
