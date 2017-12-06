//TODO
//implement line detection

using namespace std;
using namespace cv;

vector<int> detect_lines(Mat& stats){
  vector<int> lines;
  for (int i = 0; i < stats.rows; i++){
    if (density[i] >= 0.9 && hwratio[i] <= 0.1)
      lines.push_back(i);    
  }
  return lines;
}

