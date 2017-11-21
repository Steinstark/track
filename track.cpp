#include <iostream>
#include <vector>
//#include <Magick++.h> 
#include <string>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>



#include "detect_table.hpp"
#include "detect_cell.hpp"
#include "find_grid.hpp"
#include "textbox_ocr.hpp"

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

//TODO
//add RDF-logic
vector<string> logic_layout(vector<cv::Rect> cells, vector<string> content){
  return content;
}

int main(int argc, char** argv){
  if (argc != 3){
    cout << "Invalid number of arguments" << endl;
    return 0;
  }
  string pdf(argv[1]);
  string png(argv[2]);
  doc2png(pdf, png);
  vector<cv::Rect> tables = detect_tables(png);
  for (cv::Rect table : tables){
    //IMPROVEMENT
    //Read image file once for all tables in it instead of once for every table
    vector<cv::Rect> cells = detect_cells(png, table);
    vector<string> content = textbox_content(png,table, cells);
    //logic_layout(cells, content);
    for (string s : content){
      cout << s << endl;
    }
    find_grid(cells, content);
  }
 return 0;
}
