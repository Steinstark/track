#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>
#include "textbox_ocr.hpp"

using namespace std;
using namespace cv;

Pix *mat8ToPix(const cv::Mat& mat8)
{
  Pix *pixd = pixCreate(mat8.size().width, mat8.size().height, 8);
  for(int y=0; y<mat8.rows; y++) {
    for(int x=0; x<mat8.cols; x++) {
      pixSetPixel(pixd, x, y, (l_uint32) mat8.at<uchar>(y,x));
    }
  }
  return pixd;
}

vector<string> textbox_content(const Mat& bw, const vector<cv::Rect>& rv){
  const string path = "/usr/share/tesseract-ocr/tessdata";
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  if (api->Init(path.c_str(), "eng")) {
    fprintf(stderr, "Could not initialize tesseract.\n");
    exit(1);
  }
  
  Pix *image = mat8ToPix(bw);
  api->SetImage(image);
  api->SetSourceResolution(150);
  vector<string> v; 
  for (Rect r : rv){
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
