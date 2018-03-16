#include <string>
#include <iostream>
#include <cmath>

using namespace std;

void convert(int w, int h, int xdpi, int ydpi){
  double xScale = (double)150/xdpi;
  double yScale = (double)150/ydpi;
  int nw = round(w*xScale);
  int nh = round(h*yScale);
  cout << nw << " " << nh << " ";
}

int main(int argc, char** argv){
  int count = argc - 1;
  if (argc < 5 || count % 4 != 0)
    return 1;
  cout << count/4 << " ";
  for (int i = 1; i < argc; i+=4){
    convert(stoi(argv[i]), stoi(argv[i+1]), stoi(argv[i+2]), stoi(argv[i+3]));
  }
  return 0;
}
