#include <string>
#include <opencv2/opencv.hpp>

#include "detect_table.hpp"
#include "detect_cell.hpp"
#include "textbox_ocr.hpp"
#include "find_grid.hpp"

#include <Magick++.h> 

using namespace std;
using namespace cv;

void doc2png(string pdf, string png){
  using namespace Magick;
  InitializeMagick(NULL);  
  Image img(pdf);
  img.write(png);
  return;
}    

int main(int argc, char** argv){
  if (argc != 3){
    cout << "Invalid number of arguments" << endl;
    return 0;
  }
  string pdf(argv[1]);
  string png(argv[2]);
  doc2png(pdf, png);
  vector<Rect> tables = detect_tables(png);
  for (Rect& table : tables){
    vector<Rect> cells = detect_cells(png, table);
    vector<string> content = textbox_content(png, cells);   
    for (string s : content){
      cout << s << endl;
    }
    find_grid(cells, content);
  }
  return 0;
}
