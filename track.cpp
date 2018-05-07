#include <string>
#include <list>
#include <opencv2/opencv.hpp>
#include <iostream>

#include "detect_table.hpp"
#include "detect_cell.hpp"
#include "textbox_ocr.hpp"
#include "find_grid.hpp"
#include "util.hpp"
#include "image_util.hpp"

#include <Magick++.h> 

using namespace std;
using namespace cv;
using namespace Magick;

void usage(){
  cout << "Usage: track IMAGE" << endl;
  cout << "Detect tables in IMAGE" << endl;
}

int main(int argc, char** argv){
  if (argc != 2){
    usage();
    return 0;
  }
  string file(argv[1]);
  Mat img = imread(file.c_str());
  if (!img.data){
    cout << "Could not open or find image " << file << endl;
    return 1;
  }
  Mat bw = color2binary(img);
  list<Rect> tables = detect_tables(bw);
  for (Rect& table : tables){
    vector<Rect> cells = detect_cells(bw, table);
    auto grid =  find_grid(cells);
    vector<Rect> refined_cells;
    for (int i = 0; i < grid.size(); i++){
      refined_cells.push_back(grid[i].rect);
    }
    vector<string> content = textbox_content(bw, refined_cells);
    for (int i = 0; i < content.size(); i++){
      cout << grid[i] << " Text: " << content[i] << endl;
    }
    cout << endl;
  }
  return 0;
}
