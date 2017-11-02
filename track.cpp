#include <iostream>
#include <vector>
#include <Magick++.h> 
#include <string>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

using namespace std;
using namespace Magick;

class Region{
public:
  int width();
  int heigth();
  int& operator[](int x){
    return coords[x];
  }
  Region(int x1, int x2,int y1,int y2);
private:
  vector<int> coords;  
};

int Region::width(){
  return coords[1]-coords[0];
}

int Region::heigth(){
  return coords[3]-coords[2];
}
Region::Region(int x1, int x2, int y1, int y2) : coords{x1,x2,y1,y2}{}

//IMRPOVEMENT
//use stream instead of writing to file
//more accurate names
void doc2png(string file_in, string file_out){
  InitializeMagick(NULL);  
  Image pdf(file_in);
  pdf.write(file_out);
}

//TODO
//table detection implementation
Region detect_table(){
  return Region(113,153,76,92);
}

vector<Region> detect_cells(Region r){
  return vector<Region>{r};
}

vector<string> identify_content(string path, vector<Region> rv){
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  if (api->Init(NULL, "eng")) {
    fprintf(stderr, "Could not initialize tesseract.\n");
    exit(1);
  }
  Pix *image = pixRead(path.c_str());
  api->SetImage(image);
  vector<string> v;
  for (Region r : rv){
    api->SetRectangle(r[0],r[2], r.width(), r.heigth());
    v.push_back(string(api->GetUTF8Text()));    
  }
  api->End();
  pixDestroy(&image);  
  return v;
}

//TODO
//add RDF-logic
vector<string> logic_layout(vector<Region> cells, vector<string> content){
  return content;
}

int main(int argc, char** argv){
  if (argc != 3){
    cout << "Invalid numnber of arguments" << endl;
    return 0;
  }
  string pdf(argv[1]);
  string png(argv[2]);
  doc2png(pdf, png);
  Region r = detect_table();
  vector<Region> cells = detect_cells(r);
  vector<string> content = identify_content(png, cells);
  logic_layout(cells, content);
  for (string s : content){
    cout << s << endl;
  }
  return 0;
}
