#include <iostream>
#include <vector>
//nclude <Magick++.h> 
#include <string>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>


#include "detect_table.hpp"
#include "detect_cell.hpp"

using namespace std;

//TODO
//unresolved conflict with opencv and imagemagick. Need solution
//IMRPOVEMENT
//use stream instead of writing to file
void doc2png(string pdf, string png){
  /*using namespace Magick;
  InitializeMagick(NULL);  
  Image img(pdf);
  img.write(png);*/
  return;
}    

//IMPROVEMENT
//Inefficient to load image for every table (if multiple tables on the same page)

vector<string> identify_content(string path, cv::Rect table, vector<cv::Rect> rv){
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
    string add(api->GetUTF8Text());
    v.push_back(add);    
  }
  api->End();
  pixDestroy(&image);  
  return v;
}

//TODO
//add RDF-logic
vector<string> logic_layout(vector<cv::Rect> cells, vector<string> content){
  return content;
}

int main(int argc, char** argv){
  if (argc != 2){
    cout << "Invalid number of arguments" << endl;
    return 0;
  }
  //  string pdf(argv[1]);
  string png(argv[1]);
  //doc2png(pdf, png);
  vector<cv::Rect> tables = detect_tables(png);
  cv::Rect table = tables[0];
  vector<cv::Rect> cells = detect_cells(png, table);
  vector<string> content = identify_content(png,table, cells);
  //logic_layout(cells, content);

  int index = 0;
  for (string s : content){
    cout << index++ <<": " << s;
  }
  /*for (cv::Rect table : tables){
    //IMPROVEMENT
    //Read image file once for all tables in it instead of once for every table
    
    }*/
  return 0;
}
