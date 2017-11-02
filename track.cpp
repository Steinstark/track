#include <iostream>
#include <vector>
#include <Magick++.h> 
#include <string>

using namespace std;
using namespace Magick;

class Region{
public:
  int length();
  int heigth();
  Region(int x1, int x2,int y1,int y2);
private:
  vector<int> coords;  
};

Region::Region(int x1, int x2, int y1, int y2) : coords{x1,x2,y1,y2}{}


//IMRPOVEMENT
//use stream instead of writing to file
//more accurate names
void doc2png(string file_in, string file_out){
  InitializeMagick(NULL);  
  Image pdf(file_in);
  pdf.write(file_out);
}

Region detect_table(){
  return Region(0,0,0,0);
}

vector<Region> detect_cells(Region r){
  return vector<Region>{r};
}

vector<string> identify_content(vector<Region> rv){
  return vector<string>{"dummy"};
}

void logic_layout(){
  
}

int main(int argc, char** argv){
  if (argc != 3){
    cout << "Invalid numnber of arguments" << endl;
    return 0;
  }
  string in(argv[1]);
  string out(argv[2]);
  doc2png(in, out);
  Region r = detect_table();
  vector<Region> cells = detect_cells(r);
  identify_content(cells);
  logic_layout();
  return 0;
}
